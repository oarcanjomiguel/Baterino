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
 
unsigned char PadNote[6] = {52,16,66,63,40,65};         // Notas MIDI - 0 a 127
 
int PadCutOff[6] = {600,600,600,600,600,600};           // Valor Mínimo do Sensor para causar o som
 
int MaxPlayTime[6] = {90,90,90,90,90,90};               // Ciclos a passar antes da Segunda Batida ser acionada.
 
#define  midichannel    0;                              // Canal Midi
 
boolean VelocityFlag  = true;                           // Se o som será de acordo com a intensidade da Batida.
 
 
 
 
 
//*******************************************************************************************************************
// Variáveis de uso Interno                   
//*******************************************************************************************************************
 
boolean activePad[6] = {0,0,0,0,0,0};                   // Salva se os pads estao ativos ou nao.
int PinPlayTime[6] = {0,0,0,0,0,0};                     // Contador dos ciclos desde que o pad foi acionado.
 
unsigned char status;
 
int pin = 0;    
int hitavg = 0;
 
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
  for(int pin=0; pin < 6; pin++) // Percorre os Pinos Analógicos
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
      }
    }
  }
}
