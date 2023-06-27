//Morse code decoder
//by Yuriy Volkov

// Photoresistor pin №
const int photoPin = 0;

// Light level must be more then ledLevel when LED is ON, and less then ledLevel when LED is OFF
const int ledLev = 580;

//Dot signal length (milliseconds)
const int dot = 500;

//approximation (milliseconds)
const int approx = 100;

// Choose wich Morse code layout to use:
// r - russian Cyrillic
// t - russian transliteration
// i - international(english)
const char morseLayout = 'r';

//Morse code table: 1 - "dot", 2 - "dash"
const int morseCodeInt[] = {
  12, 2111, 122, 221, 211, 1, 1112, 2211, 11, 1222, 212, 1211, 
  22, 21, 222, 1221, 121, 111, 2, 112, 1121, 1111, 2121, 2221, 
  2222, 2212, 2122, 2112, 11211, 1122, 1212, 
  22222, 12222, 11222, 
  11122, 11112, 11111, 21111, 22111, 22211, 22221, 111111, 121212,
  222111, 212121, 221122, 112211, 21112, 121121, 122221, 212212, 
  21121, 211112, 112212, 122121, 11111111 };

//Decode tables layout
const char* layoutCyrillic[] = {
 "А","Б","В","Г","Д","Е","Ж","З","И","Й",
  "К","Л","М","Н","О","П","Р","С","Т","У",
  "Ф","Х","Ц","Ч","Ш","Щ","Ы","Ь","Э","Ю",
  "Я",
  "0","1","2","3","4","5","6","7","8","9",
  ".",",",":",";","!","?", "=","**","*","()",
  "/", "-", "_", "@", "ERROR" };

const char* layoutTranslit[] = {
 "A","B","V","G","D","E","Zh","Z","I","j",
  "K","L","M","N","O","P","R","S","T","U",
  "F","H","C","CH","SH","SHj","Yh","gh","Eh","Yu",
  "Ya",
  "0","1","2","3","4","5","6","7","8","9",
  ".",",",":",";","!","?", "=","**","*","()",
  "/", "-", "_", "@", "ERROR" };

const char* layoutInternational[] = {
 "A","B","W","G","D","E","V","Z","I","J",
  "K","L","M","N","O","P","R","S","T","U",
  "F","H","C","","","Q","Y","X","","",
  "",
  "0","1","2","3","4","5","6","7","8","9",
  ".",",",":",";","!","?", "=","**","*","()",
  "/", "-", "_", "@", "ERROR" };

//dash and intervals lengths (milliseconds)
int dash = dot*3;
int symbolInterval = dot;
int letterInterval = 3*dot;
int wordInterval = 5*dot; // actual interval in morse code is 7*dot

// Led on/off and "word in progress" triggers
int ledIsOn = 0;
int progress = 0;

//Timer support
unsigned long ledTime = 0;
unsigned long darkTime = 0;
unsigned long timeDiff = 0;

//Current symbol and letter codes
int symbol = 0;
int letter = 0; 

//Current light level
int light = 0;

void setup() 
{
  // put your setup code here, to run once:
  pinMode (photoPin, INPUT);
  pinMode (13, OUTPUT);
  Serial.begin(9600);  
}

void loop() 
{
  // Check led on or off
  light = analogRead (photoPin);
  
  // Checking if LED turns on
  if (light >= ledLev && ledIsOn ==0)
  {    
    ledIsOn = 1;
    digitalWrite(13, HIGH);
    ledTime = millis();
    timeDiff = ledTime - darkTime;

    // Checking interval lenghth
    if (progress==1)
    {
      if (timeDiff < dot - approx)
      {
        Serial.println("Error: interval length is to short");
      }

      // Adding new symbol to letter code, if interval was short
      else if (timeDiff < dot + approx)
      {
        letter = letter*10 + symbol;
        symbol = 0;
      }

      // Adding last symbol to letter code, decode and print letter, errase letter code (prepare for next latter)
      else if (timeDiff < dash + approx)
      {
        letter = letter*10+symbol;
        for (int i = 0; morseCodeInt[i] != '\0'; i++)
        {
          if (morseCodeInt[i] == letter)
          {
             if (morseLayout == 'r') 
             {
                Serial.println (layoutCyrillic[i]);
             }
             else if (morseLayout == 'i')
             {
                Serial.println (layoutInternational[i]);
             }
             else if (morseLayout == 't')
             {
                Serial.println (layoutTranslit[i]);
             }
             else Serial.println("Layout choise error");
             letter = 0;                              
          }
        }
      }
    }  
  }

  // Checking if LED turns off
  if (light < ledLev && ledIsOn==1)
  {
    ledIsOn = 0;
    digitalWrite(13, LOW);
    darkTime = millis();
    timeDiff = darkTime - ledTime;
    progress = 1;

    if(timeDiff < symbolInterval - approx)
    {
      Serial.println("Error: symbol length is to short");
    }

    // The symbol was "dot" case
    else if (timeDiff < symbolInterval + approx)
    {
      symbol = 1;
    }

    // The symbol was "dash" case
    else if (timeDiff < letterInterval + approx)
    {
      symbol = 2;
    }
    else
    {
      Serial.println("Word ending decoding error");
    }
  }

  //Checking if wordInterval is passed
  if (light < ledLev && progress==1 && (millis() - darkTime)> wordInterval)
  {    
    letter = letter*10+symbol;
    progress = false;
    for (int i = 0; morseCodeInt[i] != '\0'; i++)
    {
      if (morseCodeInt[i] == letter)
      {
         if (morseLayout == 'r') 
         {
            Serial.println (layoutCyrillic[i]);
         }
         else if (morseLayout == 'i')
         {
            Serial.println (layoutInternational[i]);
         }
         else if (morseLayout == 't')
         {
            Serial.println (layoutTranslit[i]);
         }
         else Serial.println("Layout choise error");
             
         Serial.println ();
         letter = 0;
         symbol =0;
         progress=0;
         ledIsOn = 0;                            
      }
    }
  }    
}
