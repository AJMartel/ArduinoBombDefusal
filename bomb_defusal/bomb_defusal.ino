#include <LiquidCrystal.h>
#include <Tone.h>
#include <Keypad.h>

#define GREENWIRE 4
#define YELLOWWIRE 5
#define REDWIRE 6

/*** CLAVIER ***/
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};

byte rowPins[ROWS] = {1, A5, A4, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A2, A1, A0}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
/*** CLAVIER ***/

/*** TONE ***/
Tone tone1;
int playDuration = 200; // how long the sound is played
int playTimes = 1; // how many times the sound is played
/*** TONE ***/

/*** LCD ***/
LiquidCrystal lcd(7,8,10,11,12,13); // the pins we use on the LCD
/*** LCD ***/

/*** LED ***/
int greenLED = 2; //green led
int redLED = 3 ; //red led
/*** LED ***/

/*** WIRES ***/
int wireToCut = 4;
int wires [] = {GREENWIRE, REDWIRE, YELLOWWIRE};
bool wiresPrinted = false; //used to print only once the "bomb hacked" effect
/*** WIRES ***/

/*** GLOBAL VARIABLES ***/
bool isArmed = false;
int currentLength = 0; //defines which number we are currently writing
char password[4]; // number of characters in our password
char entered[4]; //the user inputs for password
int passwordTries = 0;
int gameMode = 0;
char pswdHints[4] = { '#', '#', '#', '#'};

int Scount = 0; // count seconds
int Mcount = 0; // count minutes
int Hcount = 0; // count hours
long secMillis = 0; // store last time for second add
long interval = 1000; // interval for seconds

int overrideValue[2]; // the value to win the "override" mini game
int currentVoltage[2]; // the current bomb voltage in the game
int currentVoltageLenght = 0;

bool passwordFound = false;
/*** GLOBAL VARIABLES ***/

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  tone1.begin(9);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(greenLED, HIGH);

  //set up wires pin
  pinMode(GREENWIRE,OUTPUT);
  pinMode(YELLOWWIRE,OUTPUT);
  pinMode(REDWIRE,OUTPUT);

  gameInit();
}

/*********************
 * Function : loop
 * Parameters : none
 * Description :
 *  Main function in the game
 *********************/
void loop() {
  //If the correct wire is cut : win
  //If the wrongs wires are cut : lose

  bool timerMode = true; //handles timer display mode : true -> display; false -> don't display
  
  if(wireCut())
  {
    bombDefused();
  }

  if(!passwordFound){
    passwordFound = getPassword();
  }else{
    timerMode = false; //if the password has been found, following mini games doesn't need timer display
  }

  switch(gameMode)
  {
    case 0: // easy mode
      if(passwordFound)
      {
        bombDefused();
      }
      break;
    case 1: // normal mode
      if(passwordTries > 10)
        explode();
      if(passwordFound)
      {
        printWire();
      }
      break;
    case 2: // hard mode
      if(passwordTries > 5)
        explode();
      if(passwordFound)
      {
        //if(overrideGame()) // this game needs to be improved
          printWire();
      }
      break;
    default:
      break;
  } 
  
  timer(timerMode);
}

/*********************
 * Function : init
 * Parameters : none
 * Description :
 *  This function initializes the game
 *********************/
void gameInit()
{
  //call functions that setup the game
  selectWire();
  setGameMode();
  setTimer();

  //init password variables
  currentLength = 0;
  for(int i = 0; i < 4; password[i] = 0, i++);
  for(int i = 0; i < 4; pswdHints[i] = 0, i++);
  passwordTries = 0;
  //
  selectPassword();
  for(int i =0; i <2; currentVoltage[i] = 0, i++);
  for(int i =0; i <2; overrideValue[i] = random(0,9), i++);
  digitalWrite(greenLED, LOW);
}

/*********************
 * Function : setTimer
 * Parameters : none
 * Description :
 *  This function initializes the timer
 *********************/
void setTimer()
{
  switch(gameMode)
  {
    case 0:
      Scount = 0;
      Mcount = 3;
      Hcount = 0;
      break;
    case 1:
      Scount = 0;
      Mcount = 2;
      Hcount = 0;
      break;
    case 2:
      Scount = 0   ;
      Mcount = 1;
      Hcount = 0;
      break;
    default:
      //same settings as easy mode
      Scount = 0;
      Mcount = 2;
      Hcount = 0;
  }

  playDuration = 200;
  playTimes = 1;    
}

/*********************
 * Function : bombDefused
 * Parameters : none
 * Description :
 *  This function sets output for "bomb defused mode" and calls init() function to restart the game
 *********************/
void bombDefused()
{
  int i =0;
  digitalWrite(redLED, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bomb DEFUSED");

  while (i < 5) 
  {
      digitalWrite(greenLED, HIGH); // sets the LED on
      tone1.play(NOTE_G5, 90);
      delay(200); 
      digitalWrite(greenLED, LOW); // sets the LED off
      tone1.play(NOTE_G5, 90);
      delay(200);
      i++;
  }
  gameInit();
}

/*********************
 * Function : explode
 * Parameters : none
 * Description :
 *  This function sets outputs for "explosion" mode
 *********************/
void explode()
{
  int i =0;
  lcd.noCursor();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EXPLOOOOSION");
  // Horrible noise to prove the bomb has exploded
  while (i < 5) 
  {
      digitalWrite(redLED, HIGH); // sets the LED on
      tone1.play(NOTE_A2, 90);
      delay(200); 
      digitalWrite(greenLED, HIGH); // sets the LED off
      tone1.play(NOTE_A2, 90);
      delay(200);
      digitalWrite(redLED, LOW); // sets the LED on
      tone1.play(NOTE_A2, 90);
      delay(200); 
      digitalWrite(greenLED, LOW); // sets the LED off
      tone1.play(NOTE_A2, 90);
      delay(200);
      i++;
  }
  gameInit();
}

/*********************
 * Function : timer
 * Parameters : none
 * Description :
 *  overloaded function to enable default timer "displayMode" param value to true
 *********************/
void timer()
{
  timer(true);
}

/*********************
 * Function : timer
 * Parameters : none
 * Description :
 *  This function create the timer, and increments it
 *********************/
void timer(bool displayMode)
{

  // if the timer is equals to zero, the bomb goes into "explosion" mode
  if (Hcount == 0 && Scount == 0 && Mcount == 0)
  {
    explode(); 
  }

  if(displayMode)
  {
    lcd.setCursor (0,1); // sets cursor to 2nd line
    lcd.print ("Timer:");
  
    if (Hcount >= 10)
      {
        lcd.setCursor (7,1);
        lcd.print (Hcount);
      }
    if (Hcount < 10) 
      {
        lcd.setCursor (7,1);
        lcd.write ("0");
        lcd.setCursor (8,1);
        lcd.print (Hcount);
      }
  
    lcd.print (":");
  
    if (Mcount >= 10)
      {
        lcd.setCursor (10,1);
        lcd.print (Mcount);
      }
    if (Mcount < 10) 
      {
        lcd.setCursor (10,1);
        lcd.write ("0");
        lcd.setCursor (11,1);
        lcd.print (Mcount);
      }
      
    lcd.print (":");
  
    if (Scount >= 10) 
      {
        lcd.setCursor (13,1);
        lcd.print (Scount);
      }
    if (Scount < 10) 
      {
        lcd.setCursor (13,1);
        lcd.write ("0");
        lcd.setCursor (14,1);
        lcd.print (Scount);
      }
  }

  if (Hcount <0) 
    {
      Hcount = 0; 
    }

  if (Mcount <0) 
    {
      Hcount --; 
      Mcount = 59; 
    }

  if (Scount <1) // if 60 do this operation
    {
      Mcount --; // add 1 to Mcount
      Scount = 59; // reset Scount
    }

  if (Scount > 0) // do this oper. 59 times
    {
      unsigned long currentMillis = millis();

      int timeRemaining = (Hcount * 3600) + (Mcount * 60) + Scount;
    
      if(timeRemaining % 10 == 0 && timeRemaining <= 60)
        if(playDuration >= 10)
          playDuration-=1;
        
      if(timeRemaining % 10 == 0 && timeRemaining <= 30)
        playTimes++;
  
      if(currentMillis - secMillis > interval) 
        {
          for(int i = 0; i < playTimes; tone1.play(NOTE_G5, playDuration), delay(playDuration), i++);
          secMillis = currentMillis;
          Scount --; // add 1 to Scount
          digitalWrite(redLED, HIGH); // sets the LED on
          delay(10); // waits for a second
          digitalWrite(redLED, LOW); // sets the LED off
          delay(10); // waits for a second
        }
    }
}

/*********************
 * Function : wireCut
 * Parameters : none
 * Return : true is the correct wire is cut
 * Description :
 *  This function verifies if the wires are cut.
 *  If the correct wire is cut, the bomb is defused
 *  If the wrong wire is cut, the bomb "explode"
 *********************/
bool wireCut()
{
  int val = 1;
  bool correctWireCut = false;

  for(int i = 0; i < 3; i++)
  {
    digitalWrite(wires[i], LOW);
    val = digitalRead(wires[i]);
    if(val == 0)
    {
      if(wireToCut == i)
        correctWireCut = true;
      else
        explode();
    }
  }

  return correctWireCut;
}

/*********************
 * Function : selectPassword
 * Parameters : none
 * Description :
 *  This function is used to get user input when he defines the password
 *********************/
void selectPassword()
{
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Enter Code: ");
  while (currentLength < 4)
  {
    lcd.setCursor(currentLength + 6, 1);
    lcd.cursor();
    char key = keypad.getKey();
    if (key != NO_KEY)
    {
      if ((key != '*')&&(key != '#'))
      { 
      lcd.print(key);
      password[currentLength] = key;
      currentLength++;
      tone1.play(NOTE_C6, 200);
      }
    }
  }

  //if the length is equals to 4 : the password is completely entered
  if (currentLength == 4)
  {
    delay(500);
    lcd.noCursor();
    lcd.clear();
    lcd.home();
    lcd.print("You've Entered: ");
    lcd.setCursor(6,1);
    lcd.print(password[0]);
    lcd.print(password[1]);
    lcd.print(password[2]);
    lcd.print(password[3]);

    tone1.play(NOTE_E6, 200);
    delay(3000);
    lcd.clear();
    currentLength = 0;
  }
  
  passwordFound = false;
}

/*********************
 * Function : selectWire
 * Parameters : none
 * Description :
 *  This function is used to get user input when he defines the wire to cut
 *********************/
void selectWire()
{
  bool selection = false;
  while(!selection)
  {
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Select wire ");
    lcd.setCursor(0,1);
    lcd.print("1.G - 2.R - 3.Y");

    char key = keypad.getKey();
    if (key != NO_KEY)
    {
      if (key == '1')
      {
        wireToCut = 0;
        selection = true;
      }
      if (key == '2')
      {
        wireToCut = 1;
        selection = true;
      }
      if (key == '3')
      {
        wireToCut = 2;
        selection = true;
      }
    }
    //delay for a better screen display
    delay(250);
  }
}

/*********************
 * Function : setGameMode
 * Parameters : none
 * Description :
 *  This function is used to get user input when he defines the wire to cut
 *********************/
void setGameMode()
{
  bool selection = false;
  while(!selection)
  {
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Select game mode");
    lcd.setCursor(0,1);
    lcd.print("1.E - 2.M - 3.H");

    char key = keypad.getKey();
    if (key != NO_KEY)
    {
      if (key == '1')
      {
        gameMode = 0;
        selection = true;
      }
      if (key == '2')
      {
        gameMode = 1;
        selection = true;
      }
      if (key == '3')
      {
        gameMode = 2;
        selection = true;
      }
    }
    //delay for a better screen display
    delay(250);
  }
}

/*********************
 * Function : getPassword
 * Parameters : none
 * Return : true if the user has typed the correct password
 * Description :
 *  This function is used to get user input when he defines the wire to cut
 *********************/
bool getPassword()
{
  char key = keypad.getKey(); // get the key

  lcd.setCursor(0,0);
  lcd.print("Code: ");

  // give user hints about password
  lcd.setCursor(11,0);
  for(int i = 0; i <= 3; i++)
  {
    lcd.print(pswdHints[i]);
  }
           
  if (key == '#')
  {
      currentLength = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" Delete ");
  }
  else
  {                
    if (key != NO_KEY)    
    {
      lcd.setCursor(currentLength + 6, 0);
      lcd.cursor();
      
      lcd.print(key);
      entered[currentLength] = key;
      currentLength++;
      lcd.noCursor();
      lcd.setCursor(currentLength + 5, 0);
      lcd.print("*");
      lcd.setCursor(currentLength + 6, 0);
      lcd.cursor();

      passwordTips(key);
    }
  }

  if (currentLength == 4) 
  {
    //verify that the password is correct
    if (entered[0] == password[0] && entered[1] == password[1] && entered[2] == password[2] && entered[3] == password[3])
    {
      return true;
    }
    else
    {
      passwordTries++;
      currentLength = 0;
      return false;
    }
  }  

  delay(200); //delay for a better screen display
  return false;
}

/*********************
 * Function : passwordTips
 * Parameters : none
 * Description :
 *  This function gives user advice to guess the password
 *********************/
void passwordTips(char key)
{
  for(int i = 0; i <= 3; i++)
  {
    if(key == password[i])
    {
      digitalWrite(greenLED, HIGH);
      delay(200);
      digitalWrite(greenLED, LOW);
      
      if(i == currentLength-1)
      {
        pswdHints[i] = key;
        tone1.play(NOTE_C6, 200);
        delay(200);
        break;
      }
    }
  }
}

/*********************
 * Function : printWire
 * Parameters : none
 * Description :
 *  This function prints the wire the user has to cut
 *********************/
void printWire()
{
  char* wireToPrint;
  
  lcd.clear();
  lcd.setCursor(0,0);
  
  if(!wiresPrinted){
    for(int i = 0; i < 16 ; lcd.print("#"), delay(250), i++);
    lcd.setCursor(0,1);
    lcd.print("accessing");
    for(int i = 0; i < 5 ; lcd.print("."), i++, delay(500));
    wiresPrinted = true;
  }

  switch(wireToCut){
    case 0:
      wireToPrint = "green wire";
      break;
    case 1:
      wireToPrint = "red wire";
      break;
    case 2:
      wireToPrint = "yellow wire";
      break;
  }

  lcd.setCursor(0,0);
  lcd.print(wireToPrint);
  lcd.setCursor(0,1);
  lcd.print("powers the bomb");

  delay(200); //delay for a better screen display
}


/*********************
 * Function : overrideGame
 * Parameters : none
 * Description :
 *  This function handles the override mini-game
 *  The player has to find the correct value to pass throught bomb security
 *  This game is basically a guessing game
 *********************/
bool overrideGame()
{
  char key = keypad.getKey(); // get the key

  if (key != NO_KEY)    
  {     
      currentVoltage[currentVoltageLenght] =  (int) key - 48; //convert ascii char to it's integer value
      currentVoltageLenght++;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("vcc val: ");
  for(int i = 0; i < 2; lcd.print(currentVoltage[i]), i++);
  
  if(currentVoltageLenght == 2)
  {
    if(currentVoltage[0] == overrideValue[0] && currentVoltage[1] == overrideValue[1])
    {
      return true; // the player has achieved the override mini-game
    }
    else
    {
      int currentTmp = currentVoltage[0] * 10 + currentVoltage[1];
      int overrideTmp = overrideValue[0] * 10 + overrideValue[1];
      if(currentTmp < overrideTmp)
      {
        lcd.setCursor(0,1);
        lcd.print("Increase +");
        delay(1000);
        currentVoltageLenght = 0;
      }
      else
      {
        lcd.setCursor(0,1);
        lcd.print("Dicrease -");
        delay(1000);
        currentVoltageLenght = 0;
      }
    }
  }

  delay(200);
  return false;
}

