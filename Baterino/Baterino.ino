//  *****************************************************************************************************************
//  *                                                                                                               *
//  *                                     arduinodrum.wordpress.com                                                 *
//  *                                                                                                               *
//  *                                   Bateria Eletrônica com Arduino                                              *
//  *                                         Diego Siena - 2014                                                    *
//  *                                                                                                               *
//  *****************************************************************************************************************
 
//*******************************************************************************************************************
// Variáveis de Definição do Usuário.                      
//*******************************************************************************************************************

#define PADCUTOFF 400
#define MAXPLAYTIME 30
#define MINCHAN 0
#define MAXCHAN 3
unsigned char PadNote[6] = {36,55,40,63,40,65};         // Notas MIDI - 0 a 127
 
int PadCutOff[6] = {PADCUTOFF,PADCUTOFF,PADCUTOFF,PADCUTOFF,PADCUTOFF,PADCUTOFF};           // Valor Mínimo do Sensor para causar o som
 
int MaxPlayTime[6] = {MAXPLAYTIME,MAXPLAYTIME,MAXPLAYTIME,MAXPLAYTIME,MAXPLAYTIME,MAXPLAYTIME};               // Ciclos a passar antes da Segunda Batida ser acionada.
 
#define  midichannel    0;                              // Canal Midi
 
boolean VelocityFlag  = false;                           // Se o som será de acordo com a intensidade da Batida.
 
unsigned char nota = 0; 
 
 
 
//*******************************************************************************************************************
// Variáveis de uso Interno                   
//*******************************************************************************************************************
 
boolean activePad[6] = {0,0,0,0,0,0};                   // Salva se os pads estao ativos ou nao.
int PinPlayTime[6] = {0,0,0,0,0,0};                     // Contador dos ciclos desde que o pad foi acionado.
 
unsigned char status;
 
int pin = 0;    
int hitavg = 0;
unsigned long seg=0;
unsigned long segAnterior = 0;
//*******************************************************************************************************************
// Setup                       
//*******************************************************************************************************************
 
void setup()
{
  Serial.begin(57600);                              
}
 
//*******************************************************************************************************************
// Main Program                
//*******************************************************************************************************************
 
void loop()
{
  for(int pin=MINCHAN; pin < MAXCHAN; pin++) // Percorre os Pinos Analógicos
  {
    hitavg = analogRead(pin);  // Lê o Valor do Sensor                            
 
    if((hitavg > PadCutOff[pin]))  // Verifica se o valor pego pelo sensor é maior que o Valor minimo para causar o Som
    {
      if((activePad[pin] == false))  // Verifica se o Pad já está sendo executado.
      {
        if(VelocityFlag == true)  // Verifica se o som será de acordo com a Intensidade da Batida, para gerar o Sinal Midi.
        {
//          hitavg = 127 / ((1023 - PadCutOff[pin]) / (hitavg - PadCutOff[pin]));    // With full range (Too sensitive ?)
          hitavg = (hitavg / 8) -1 ;                                                 // Upper range
        }
        else
        {
          hitavg = 127;
        }
 
        MIDI_TX(144,PadNote[pin],hitavg); // Joga o SInal MIDI
        //MIDI_TX(144,nota,hitavg); // Joga o SInal MIDI
        PinPlayTime[pin] = 0;  //Seta o Ciclo para '0'
        activePad[pin] = true;  // Altera o Pad para Ativo.
      }
      else
      {
        PinPlayTime[pin] = PinPlayTime[pin] + 1; // Caso o Pad ja esteja ativo, incrementa 1 Ciclo.
      }
    }
    else if((activePad[pin] == true)) // ESTA SEGUNDA PARTE É RESPONSÁVEL APENAS POR INCREMENTAR OS CICLOS E ATIVAR/DESATIVAR OS PADS.
    {
      PinPlayTime[pin] = PinPlayTime[pin] + 1;
     
      if(PinPlayTime[pin] > MaxPlayTime[pin])
      {
        activePad[pin] = false;
        MIDI_TX(128,PadNote[pin],127);
        //MIDI_TX(128,nota,127);
      }
    }
  }
  
    seg = millis()/300;
    if(seg!=segAnterior)
{
  nota++;
  segAnterior = seg;
  if(nota==127) nota = 0;
  //MIDI_TX(144,nota,127); // Joga o SInal MIDI
}
   // nota = ((seg)*127)/60;
    
 
}

//*******************************************************************************************************************
// Transmit MIDI Message
//*******************************************************************************************************************
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY) 
{
  status = MESSAGE + midichannel;
  Serial.write(status);
  Serial.write(PITCH);
  Serial.write(VELOCITY);

}
