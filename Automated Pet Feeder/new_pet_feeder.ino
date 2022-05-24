/*
    Program to control a menu using a screen on an arduino
    The program controls processes for an automated pet feeder
    Group 06 Senior Design Fall 2021
*/

// Includes Libs needed to make the menus and graphics
#include "MCUFRIEND_kbv.h" // Graphics
#include "Adafruit_GFX.h"  // Core graphics library
#include "TimeLib.h"       // For making the clock
#include "HX711.h"         // For the Load cell
#include <Servo.h>         // To turn the servo


// Define the pins that the program needs to communicate about the LCD board
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define LCD_CS A3    // Chip Select goes to Analog 3
#define LCD_CD A2    // Command/Data goes to Analog 2
#define LCD_WR A1    // LCD Write goes to Analog 1
#define LCD_RD A0    // LCD Read goes to Analog 0

// Assign human-readable names to some common 16-bit color values:
// These RGB values us a RGB565 format where each number is the number of bits for the color
// Can just use a color picker to add more colors
#define BLACK 0x0000 // Black is the main background color
#define BLUE 0x001F  // Blue is the border of the background
#define RED 0xF800
#define ORANGE 0xeba4
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF // Text is white
#define BROWN 0xc329
#define GRAY 0x7411 // Currently selected text is highlighted gray
#define PINK 0xec34

// Define things for the load cell
#define calibration_factor -177000.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN 30
#define LOADCELL_SCK_PIN 31

// ==================================================================
//                          Pin Setup
// ==================================================================

// Load Cell Pins:
#define calibration_factor -177000.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN  51        // 3 <---> 51
#define LOADCELL_SCK_PIN  50         // 2 <---> 50

// Button Pins:
// int left_button = 31; // Changed due to issues
int left_button = 30;
int right_button = 33;
int top_button = 35;
int bottom_button = 37;
int go_button = 39;
int back_button = 41;


// Hold up to five feeding times on up to 100 pages
String feeding_times[5][10];
// time type variable that holds our current time after we set the clock
time_t current_time;
// Define the state
String state = "";
// Boolean variable to tell us if we just changed states to help us not re-print the menus every single loop
bool state_change = false;
// Buttons will be read to response
String response = "";
// Create grid variables
int main_menu_grid = 0;
// Set feeding time grid variables
int sft_vert_pos = 0;
int sft_horiz_pos = 0;
// Page number for set feeding times menu (state 2)
int sft_page_number = 0;
// When we change pages in the set feeding times menu we want to clear old items
bool sft_page_change = false;
// How many items are on our set feeding time page
int max_item_bound = 0;
// Variables that represent time in the set menu time
int sft_day_num = 0;
int sft_hour = 8;
int sft_minute = 0;
String sft_am_pm = "AM";
// Tell us to not run a feeding event until we are done with our current one 
// Also wait to run events until we reach the next minute
bool wait_one_min = false;
// variable for checking is a minute has passed since we started dispensing food
long time_check = 0;
// variable to check if 10 seconds have passed, so we can check the load cell
long consumption_time_check = 0;
// Will want to wain until we are done dispensing before we dispense again
bool wait_for_motor_finish = false;
// Calories per meal
double cal_per_meal = 0;
// Save the user's prevous input for calories in case they go back
double old_cal_per_meal = 0;
// // Calories per cup
// int cal_per_cup = 0;
// Calories per gram
double cal_per_gram = 0;
// Old calories per gram
double old_cal_per_gram = 0;
// Calories eaten today
double cal_consumed_day = 0;
// Calories eaten this week
double cal_consumed_week = 0;
// Store a value once the motor is done so we can track consumption
double motor_end_load = 0.0;
// Store grams removed from our load to a variable that resets every day
double daily_consumption = 0.0;
// Store grams removed from our load to a variable that resets every week
double weekly_consumption = 0.0;
// Clock times
String day_1 = "";
String hour_1 = "";
String min_1 = "";
String am_pm_1 = "";
String date_ = "";
// Previous clock times
String old_day_1 = "";
String old_hour_1 = "";
String old_min_1 = "";
String old_am_pm_1 = "";
String old_date_ = "";
// Print time again during state changes
bool print_time_state = false;

//
//  GLOBAL VARIABLES FOR SERVO AND LOAD CELL
//
const int liftservo = 22; // 9
int deg = 0;
int right = 1;

int jam = 0;
int degold = 0;
int degnew = 0;

float weightupper = 0.00;
float weightlower = 0.00;

float weightold = 0.00;

int outoffood = 0;

int weight = 0;

// Create an object called tft from the MCUFRIEND_kbv class
MCUFRIEND_kbv tft;
// Create load cell object
HX711 scale;
// Create the servo object
Servo lift;



void setup()
{
    // Set the baud rate
    // What would be the best baud rate for us?
    Serial.begin(115200);

    // Most likey resets the screen when program turns on
    tft.reset();

//=====================================> Uncomment and Comment the load cell and servo stuff to test the menus <==================================================//
    // Load cell setup
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
    scale.tare();                        //Assuming there is no weight on the scale at start up, reset the scale to 0

    Serial.println("Readings:");

    // Servo Setup
    lift.attach(liftservo);
    lift.write(deg);
    pinMode(liftservo, OUTPUT);

    // Set the mode for the buttons
    // Mode should be set to input pull up to avoid ambiguous readings
    pinMode(left_button, INPUT_PULLUP);
    pinMode(bottom_button, INPUT_PULLUP);
    pinMode(right_button, INPUT_PULLUP);
    pinMode(top_button, INPUT_PULLUP);
    pinMode(go_button, INPUT_PULLUP);
    pinMode(back_button, INPUT_PULLUP);


    // Code that checks if our screen has the right driver
    if (1 == 1) // If statement is only here to condense the code
    {
        uint16_t identifier = tft.readID();
        if (identifier == 0x9325)
        {
            Serial.println(F("Found ILI9325 LCD driver"));
        }
        else if (identifier == 0x9328)
        {
            Serial.println(F("Found ILI9328 LCD driver"));
        }
        else if (identifier == 0x4535)
        {
            Serial.println(F("Found LGDP4535 LCD driver"));
        }
        else if (identifier == 0x7575)
        {
            Serial.println(F("Found HX8347G LCD driver"));
        }
        else if (identifier == 0x9595)
        {
            Serial.println(F("Found HX8347-I LCD driver"));
        }
        else if (identifier == 0x4747)
        {
            Serial.println(F("Found HX8347-D LCD driver"));
        }
        else if (identifier == 0x8347)
        {
            Serial.println(F("Found HX8347-A LCD driver"));
        }
        else if (identifier == 0x9341)
        {
            Serial.println(F("Found ILI9341 LCD driver"));
        }
        else if (identifier == 0x7783)
        {
            Serial.println(F("Found ST7781 LCD driver"));
        }
        else if (identifier == 0x8230)
        {
            Serial.println(F("Found UC8230 LCD driver"));
        }
        else if (identifier == 0x8357)
        {
            Serial.println(F("Found HX8357D LCD driver"));
        }
        else if (identifier == 0x0101)
        {
            identifier = 0x9341;
            Serial.println(F("Found 0x9341 LCD driver"));
        }
        else if (identifier == 0x7793)
        {
            Serial.println(F("Found ST7793 LCD driver"));
        }
        else if (identifier == 0xB509)
        {
            Serial.println(F("Found R61509 LCD driver"));
        }
        else if (identifier == 0x9486)
        {
            Serial.println(F("Found ILI9486 LCD driver"));
        }
        else if (identifier == 0x9488)
        {
            Serial.println(F("Found ILI9488 LCD driver"));
        }
        else
        {
            Serial.print(F("Unknown LCD driver chip: "));
            Serial.println(identifier, HEX);
            Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
            Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
            Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
            Serial.println(F("If using the breakout board, it should NOT be #defined!"));
            Serial.println(F("Also if using the breakout, double-check that all wiring"));
            Serial.println(F("matches the tutorial."));
            identifier = 0x9486;
        }
        tft.begin(identifier);
    }
    // Populate the feeding times array with needed options
    feeding_times[0][0] = "Add a new feeding time";

    // Create background for the screen and set things up
    tft.setRotation(1);
    tft.fillRect(0, 0, 480, 320, BLACK);
    tft.setTextSize(3);


    // // Create cool back ground
    tft.fillRect(0, 0, 480, 107, CYAN);
    tft.fillRect(0, 108, 480, 106, BLACK);
    tft.fillRect(0, 214, 480, 107, GRAY);

    // // Intro Screen to Flex our colors

    // Make cool cat
    //tft.fillRect(0, 0, 480, 320, BLACK);
    // Head 
    tft.fillCircle(240, 160, 100, BROWN);  
    // Ears
    tft.fillTriangle(132.38, 64.17, 148.80, 118.98, 175.26, 83.78, BROWN);
    tft.fillTriangle(148.80, 99.43, 142.89, 77.25, 162.53, 83.78, PINK);
    tft.fillTriangle(304.74, 83.78, 331.20, 118.98, 347.62, 64.16, BROWN);
    tft.fillTriangle(317.47, 83.78, 337.11, 77.25, 331.20, 99.43, PINK);
    // Nose
    tft.fillTriangle(227.50, 172.10, 240, 182.10, 252.20, 172.10, PINK);
    // Whiskers
    // Left
    tft.drawLine(107.90, 127.27, 190, 149.27, GRAY);
    tft.drawLine(105, 159.27, 190, 159.27, GRAY);
    tft.drawLine(107.90, 191.27, 190, 169.27, GRAY);
    // Right
    tft.drawLine(372.10, 127.27, 290, 149.27, GRAY);
    tft.drawLine(375, 159.27, 290, 159.27, GRAY);
    tft.drawLine(372.10, 191.27, 290, 169.27, GRAY);

    // Show the cat for 3 seconds
    delay(10000);

    // // Display Group 6, Senior Design 1, Fall 2021
    tft.fillRect(0, 0, 480, 320, BLACK);
    // tft.fillRect(0, 0, 480, 107, CYAN);
    // tft.fillRect(0, 0, 108, 213, BLACK);
    // tft.fillRect(0, 0, 214, 320, GRAY);
    tft.setTextColor(ORANGE, BLACK);
    tft.setTextSize(4);
    tft.setCursor(150, 100);
    tft.println("Group 6");
    tft.setCursor(65, 150);
    tft.println("Senior Design 1");
    tft.setCursor(135, 200);
    tft.println("Fall 2021");
    tft.drawRect(50, 80, 380, 180, WHITE);
    delay(10000);

    // // Tell the user that we are going to do a few settings first
    tft.fillRect(0, 0, 480, 320, BLACK);
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(105, 100);
    tft.println("Let's Set A");
    tft.setCursor(100, 150);
    tft.println("Few Settings");
    tft.setCursor(180, 200);
    tft.println("First");
    delay(10000);
    tft.fillRect(0, 0, 480, 320, BLACK);


    // ------------------------- Force the User to Enter Required User Input at the Start -----------------------------------------
    if (1 == 1) // If statement to condense the forced user input in VS Code
    {
        // Set up the clock menu here because we do it in the loop later
        tft.fillRect(0, 0, 480, 320, BLACK);
        tft.setTextSize(4);
        tft.setTextColor(ORANGE, BLACK);
        tft.setCursor(140, 20);
        tft.println("Set Clock");
        tft.drawRect(40, 10, 405, 50, WHITE);
        tft.setCursor(70, 175);
        tft.setTextSize(3);
        tft.setTextColor(WHITE, GRAY);
        tft.print("0" + (String)1);
        tft.setTextColor(WHITE, BLACK);
        tft.print("/");
        tft.setCursor(125, 175);
        tft.println("0" + (String)1 + "/");
        tft.setCursor(180, 175);
        tft.println((String)2021);
        tft.setCursor(280, 175);
        tft.println("0" + (String)2 + ":");
        tft.setCursor(330, 175);
        tft.println("0" + (String)0);
        tft.setCursor(375, 175);
        tft.println("AM");
        // The set_clock function is in charge of changing things
        set_clock(true);

        // Ask the user for calories per gram ------------------------------------------------------------------------------------------

        // Print menu on the screen:
        state_change = false;
        tft.fillRect(0, 0, 480, 320, BLACK);
        // Make the header
        tft.setTextSize(4);
        tft.setTextColor(ORANGE, BLACK);
        tft.drawRect(40, 10, 400, 90, WHITE);
        tft.setCursor(70, 20);
        tft.println("Enter Calories");
        tft.setCursor(140, 60);
        tft.println("Per Gram");
        // Make the scrollable menu
        tft.setTextColor(WHITE, GRAY);
        tft.setTextSize(4);
        tft.setCursor(195, 200);
        //tft.print(cal_per_gram);
        old_cal_per_gram = cal_per_gram;

        // Control menu based off user input:
        double past_cal_per_gram;   // Variable to make lcd printing look better, (otherwise we print over and over)
        response = "";      // Clear the respnose from "go" so we don't skip the next part
        while (response != "go")
        {
            response = ""; 
            response = read_buttons();
            if (response == "top")
            {
                cal_per_gram = cal_per_gram + 0.25;
            }
            else if (response == "bottom")
            {
                cal_per_gram = cal_per_gram - 0.25;;
            }

            if (cal_per_gram <= 0)
            {
                cal_per_gram = 0;
            }

            if (past_cal_per_gram != cal_per_gram)
            {
                tft.setTextColor(WHITE, GRAY);
                tft.setTextSize(4);
                tft.setCursor(195, 200);
                tft.fillRect(150, 160, 280, 260, BLACK);
                tft.print(cal_per_gram);
            }
            past_cal_per_gram = cal_per_gram;
        }

        // Ask the user for calories per meal ---------------------------------------------------------------------------------------

        // Print the menu on the screen:
        state_change = false;
        tft.fillRect(0, 0, 480, 320, BLACK);
        // Make the header
        tft.setTextSize(4);
        tft.setCursor(60, 20);
        tft.setTextColor(ORANGE, BLACK);
        tft.drawRect(40, 10, 405, 90, WHITE);
        tft.println("Choose Calories"); // Per Meal
        tft.setCursor(140, 60);
        tft.println("Per Meal");
        // Make the scrollable menu
        tft.setTextColor(WHITE, GRAY);
        tft.setTextSize(4);
        tft.setCursor(180, 200);
        //tft.print(cal_per_meal);
        // Save the previous value in case the user goes back
        old_cal_per_meal = cal_per_meal;

        // Control user input:
        double past_cal_per_meal;
        response = "";      // Clear the respnose from "go" so we don't skip the next part
        while (response != "go")
        {
            response = "";
            // read button input
            response = read_buttons();
            if (response == "top")
            {
                cal_per_meal = cal_per_meal + 5;
            }
            else if (response == "bottom")
            {
                cal_per_meal = cal_per_meal - 5;;
            }

            if (cal_per_meal <= 0)
            {
                cal_per_meal = 0;
            }

            // Print the updated calories per meal
            if (past_cal_per_meal != cal_per_meal)
            {
            tft.setTextColor(WHITE, GRAY);
            tft.setTextSize(4);
            tft.setCursor(180, 200);
            tft.fillRect(160, 160, 280, 260, BLACK);
            tft.print(cal_per_meal);
            }

            past_cal_per_meal = cal_per_meal;
        }
    }
    // ------------------------------------------------- End of Forced User Input -----------------------------------------------//

    // Start our state at 1 (main menu)
    state = "1";
    state_change = true;
    // Clear button input up top for things to work right on the start
    response = "";
    // Start a timer that we check every 10 seconds to measure the consumption
    consumption_time_check = millis();
}

// Function to turn on the servo once the clock time equals the feeding time
void run_servo()
{

    Serial.print("Reading: ");
    Serial.print((scale.get_units()) * -453.592, 2); //scale.get_units() returns a float
    Serial.print(" grams");                          //You can change this to kg but you'll need to refactor the calibration_factor
    // Serial.println();
    // Serial.println("ENTER AMOUNT OF GRAMS TO DISPENSE");
    // while (Serial.available() == 0)
    // {
    // }

    // String response = "";
    // response = Serial.readString();
    // Serial.println(response);
    //int target = response.toInt();
    double target = cal_per_meal / cal_per_gram;
    target = target - 0;
    Serial.println(target);
    float weight = 0;

    while (weight < target-5)
    {
        Serial.print("Weight: ");
        weight = (scale.get_units()) * -453.592;
        Serial.print(weight, 2); //scale.get_units() returns a float  //You can change this to kg but you'll need to refactor the calibration_factor
        Serial.println();
        Serial.print("Target: ");

        Serial.println(target);

        weightlower = weight - 5.0;
        weightupper = weight + 5.0;

        if (deg > 0 && deg < 180 && right == 1)
        {
            deg = deg + 5;
            degold = degnew;
            degnew = deg;
            lift.write(deg);
            Serial.println(deg);
            delay(50);
        }
        if (deg > 0 && deg < 180 && right == 0)
        {
            deg = deg - 5;
            degold = degnew;
            degnew = deg;
            lift.write(deg);
            Serial.println(deg);
            delay(50);
        }
        if (deg == 0)
        {
            right = 1;
            deg = deg + 5;
            degold = degnew;
            degnew = deg;
            lift.write(deg);
            Serial.println(deg);
        }
        if (deg == 180)
        {
            right = 0;
            deg = deg - 5;
            degold = degnew;
            degnew = deg;
            lift.write(deg);
            Serial.println(deg);
        }
        if (weight > target)
        {
            break;
        }
        if (degnew == degold)
        {
            jam += 1;
        }
        else
        {
            jam = 0;
        }

        if (jam > 10)
        {
            Serial.println("JAM DETECTED");
        }
        if (weightold > weightlower && weightold < weightupper)
        {
            outoffood = outoffood + 1;
        }
        else
        {
            outoffood = 0;
        }

        if (outoffood > 1000)
        {
            Serial.println("OUT OF FOOD");
        }

        weightold = weight;
    }

    bool done = true;
    jam = 0;
    weightold = 0;
    weightupper = 0;
    weightlower = 0;


    // Once the servo has stopped allow us to start a new feeding time again
    wait_for_motor_finish = false;
    // Wait 45 seconds minute after we dispense food
    time_check = millis();
    wait_one_min == true;
    // Take store a load cell reading in a variable to help monitor consumption
    //motor_end_load = (double)weight;
    motor_end_load = (double)target;
}

void loop()
{
    
    // Read button input, don't stop program
    response = read_buttons();
    Serial.println("---------------------------------------------------------");
    Serial.println(response);
    Serial.println("Daily consumption is: " + (String)daily_consumption);
    Serial.println("Weekly consumption is: " + (String)weekly_consumption);
    Serial.println("Last motor load is: " + (String)motor_end_load);
    Serial.println("Wait for motor to finish is: " + (String)wait_for_motor_finish); // should be false
    Serial.println("Wait for minute is: " + (String)wait_one_min); // should be false
    Serial.println("Consumption waiting time: " + (String)(millis() - consumption_time_check));
    Serial.println("Weight: " + (String)weight);
    Serial.println("State: " + (String)state);



    // Every 10 seconds check if the add calories to consumption based off the load cell readings
    if (millis() - consumption_time_check > 10000)
    {
        // Measure the weight on the load cell
        double weight = (scale.get_units()) * -453.592;

        // Don't add calories to consumption if the difference is neglibible
        if ((motor_end_load - weight) > 1.0)
        {
            // If weight is removed we add the calories from that dog food to consumption
            daily_consumption = daily_consumption + (motor_end_load - weight) * cal_per_gram;
            weekly_consumption = weekly_consumption + (motor_end_load - weight) * cal_per_gram;
            // Reset our timer
            
            // Reset motor end load (the past value) to the load cell reading (our current value)
            motor_end_load = weight;
        }

        consumption_time_check = millis();
    }

    // Reset the daily consumption at the end of each day
    if ((int)hour() == 23 && (int)minute() == 59 && (int)second() > 50)
    {
        daily_consumption = 0.0;
    }

    // Reset the weekly consumption at the end of the week
    if ((int)hour() == 23 && (int)minute() == 59 && (int)second() > 50 && (int)weekday() == 7)
    {
        weekly_consumption = 0.0;
    }

    // Confirm that about minute has passed
    if (millis() - time_check > 61000)
    {
        wait_one_min = false;
    }
    else
    {
        wait_one_min = true;
    }
    
    // Loop though our feeding time array to see if we should turn the motor
    int t_week_day = 0;
    int t_min = 0;
    int t_hour = 0;
    if (wait_for_motor_finish == false && wait_one_min == false)
    {
        // Loop though pages first
        for (int i = 0; i < 10; i++)
        {
            // Then loop though items
            for (int j = 0; j < 5; j++)
            {
                // Convert our saved time into formats that match the time library
                // Go the next loop iteration if our time is invalid
                if (feeding_times[j][i] == "" || feeding_times[j][i] == "Add a new feeding time")
                {
                    continue;
                }

                // Pull the minute from the our feeding_time
                t_min = (int)getValue(feeding_times[j][i], ' ', 1).substring(3, 5).toInt();

                // Pull the hour, the hour gets pulled in military format 0-23
                t_hour = (int)getValue(feeding_times[j][i], ' ', 1).substring(0, 2).toInt();
                // Add 12 if we're in PM
                if (isPM() == true)
                {
                    t_hour = t_hour + 12;
                }

                // Convert the week day string to a number to compare
                Serial.println(getValue(feeding_times[j][i], ' ', 0));
                if (getValue(feeding_times[j][i], ' ', 0) == "Sunday")
                {
                    t_week_day = 1;
                }
                else if (getValue(feeding_times[j][i], ' ', 0) == "Monday")
                {
                    t_week_day = 2;
                }
                else if (getValue(feeding_times[j][i], ' ', 0) == "Tuesday")
                {
                    t_week_day = 3;
                }
                else if (getValue(feeding_times[j][i], ' ', 0) == "Wednesday")
                {
                    t_week_day = 4;
                }
                else if (getValue(feeding_times[j][i], ' ', 0) == "Thursday")
                {
                    t_week_day = 5;
                }
                else if (getValue(feeding_times[j][i], ' ', 0) == "Friday")
                {
                    t_week_day = 6;
                }
                else if (getValue(feeding_times[j][i], ' ', 0) == "Saturday")
                {
                    t_week_day = 7;
                }
                else if (getValue(feeding_times[j][i], ' ', 0) == "Everyday")
                {
                    t_week_day = -1;
                }

                // Now compare the weekday, hour, and minute to see if we run the motor
                if ((int)weekday() == t_week_day && t_hour == hour() && t_min == minute() || t_week_day == -1 && t_hour == hour() && t_min == minute())
                {
                    wait_for_motor_finish = true;

                    // Run the servo to dispense food
                    run_servo();
                }
                // Reset the values back to 0 for the next comparision
                t_week_day = 0;
                t_hour = 0;
                t_week_day = 0;
            }
        }
    }

    //==============================================================On State Changes Print Menus to the LCD=================================================//

    // Clearing the state here fixes the issue where button inputs get read multiple times and we move between multiple menus at a time
    if (state_change == true)
    {
        response = "";
    }
    
    // Print the menu on the LCD when changing states
    if (state == "1" && state_change == true)
    {
        state_change = false;
        tft.fillRect(0, 0, 480, 310, BLACK);
        tft.setTextSize(4);
        tft.setTextColor(ORANGE, BLACK);
        tft.setCursor(140, 20);
        tft.println("Main Menu");
        tft.drawRect(40, 10, 405, 50, WHITE);
        reset_main_menu();
        // Select the first item
        tft.setTextSize(3);
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(20, 80);
        tft.println("Set Feeding Times");
        // Reset the grid value
        main_menu_grid = 0;
        print_time_state = true;
    }

    // When changing states reset the background
    if (state == "2" && state_change == true)
    {
        // ALWAYS change the state_change back to false
        state_change = false;
        // Create the black background
        tft.fillRect(0, 0, 470, 320, BLACK);
        // Display the header
        tft.setCursor(40, 20);
        tft.setTextColor(ORANGE, BLACK);
        tft.setTextSize(4);
        tft.drawRect(20, 10, 440, 50, WHITE);
        tft.println("Set Feeding Times");
        tft.setTextSize(3);
        // Print all of the items to the menu
        reset_feeding_time_menu();
        // Highlight add a new feeding time
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(20, 80);
        tft.println(feeding_times[0][0]);
        tft.setTextColor(WHITE, BLACK);
        // Reset the grid positions back to 0 on a state change
        sft_vert_pos = 0;
        sft_horiz_pos = 0;
        sft_page_number = 0;
    }

    if (state == "2a" && state_change == true)
    {
        // Create the screen background
        state_change = false;
        tft.fillRect(0, 0, 480, 320, BLACK);
        tft.setTextSize(3);

        // Reset the time variables on a state change
        sft_day_num = 0;
        sft_hour = 8;
        sft_minute = 0;
        sft_am_pm = "AM";

        // Start the day as everyday and time as 8:00 AM
        tft.setCursor(80, 150);
        tft.setTextColor(WHITE, GRAY);
        tft.println("Everyday");
        tft.setTextColor(WHITE);
        tft.setCursor(242, 150);
        tft.println(" 0" + (String)sft_hour + ":");
        tft.setCursor(310, 150);
        tft.print("0" + (String)sft_minute);
        tft.setCursor(350, 150);
        tft.print(sft_am_pm);
    }

    if (state == "2b" && state_change == true)
    {
        state_change = false;
        tft.fillRect(0, 0, 480, 320, BLACK);
        tft.setTextSize(4);
        tft.setCursor(20, 100);
        tft.setTextColor(GREEN, BLACK);
        tft.println("Press GO to delete");
        tft.setCursor(20, 200);
        tft.setTextColor(RED, BLACK);
        tft.println("Press BACK to exit");
        tft.setTextColor(WHITE, BLACK);
        tft.setTextSize(3);
    }

    if (state == "3" && state_change == true)
    {

        state_change = false;

        tft.fillRect(0, 0, 480, 320, BLACK);
        tft.setTextSize(4);
        tft.setTextColor(ORANGE, BLACK);
        tft.setCursor(140, 20);
        tft.println("Set Clock");
        tft.drawRect(40, 10, 405, 50, WHITE);
        // Month, day, and year in the format mm, dd, yyyy
        tft.setCursor(70, 175);
        // Highlight the first part of the dates
        tft.setTextSize(3);
        tft.setTextColor(WHITE, GRAY);
        tft.print("0" + (String)1);
        tft.setTextColor(WHITE, BLACK);
        // Go back to not highlighting
        tft.print("/");
        tft.setCursor(125, 175);
        tft.println("0" + (String)1 + "/");
        tft.setCursor(180, 175);
        tft.println((String)2021);
        tft.setCursor(280, 175);
        tft.println("0" + (String)2 + ":");
        tft.setCursor(330, 175);
        tft.println("0" + (String)0);
        tft.setCursor(375, 175);
        tft.println("AM");
    }

    if (state == "4" && state_change == true)
    {
        state_change = false;
        tft.fillRect(0, 0, 480, 320, BLACK);
        // Make the header
        tft.setTextSize(4);
        tft.setCursor(60, 20);
        tft.setTextColor(ORANGE, BLACK);
        tft.drawRect(40, 10, 405, 90, WHITE);
        tft.println("Choose Calories"); // Per Meal
        tft.setCursor(140, 60);
        tft.println("Per Meal");
        // Make the scrollable menu
        tft.setTextColor(WHITE, GRAY);
        tft.setTextSize(4);
        tft.setCursor(180, 200);
        tft.print(cal_per_meal);
        // Save the previous value in case the user goes back
        old_cal_per_meal = cal_per_meal;
    }

    if (state == "5" && state_change == true)
    {
        state_change = false;
        tft.fillRect(0, 0, 480, 320, BLACK);
        // Make the header
        tft.setTextSize(4);
        tft.setTextColor(ORANGE, BLACK);
        tft.drawRect(40, 10, 400, 90, WHITE);
        tft.setCursor(70, 20);
        tft.println("Enter Calories");
        tft.setCursor(140, 60);
        tft.println("Per Gram");
        // Make the scrollable menu
        tft.setTextColor(WHITE, GRAY);
        tft.setTextSize(4);
        tft.setCursor(195, 200);
        tft.print(cal_per_gram);
        old_cal_per_gram = cal_per_gram;
    }

    if (state == "6" && state_change == true)
    {
        state_change = false;
        tft.fillRect(0, 0, 480, 320, BLACK);
        // Make the header
        tft.setTextSize(4);
        tft.setTextColor(ORANGE, BLACK);
        tft.setCursor(40, 20);
        tft.println("Calories Consumed");
        tft.drawRect(20, 10, 445, 50, WHITE);
        // tft.setCursor(140, 60);
        // tft.println("Consumption");
        // Display consuption
        tft.setTextSize(3);
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(110, 150);
        tft.print("This week: " + (String)weekly_consumption);
        tft.setCursor(140, 190);
        tft.print("Today: " + (String)daily_consumption);
        
    }

    //=======================================================Start Entering Different States==================================================================//

    // We want to display the time regardless of button input if we are in the main menu (State 1)
    if (state == "1")
    {
        // Pull todays date
        date_ = (String)day();

        if (isPM() == true)
        {
            //hour_1 = (String)((int)hour() - 12);
            am_pm_1 = "PM";
        }
        else
        {
            //hour_1 = (String)((int)hour());
            am_pm_1 = "AM";
        }

        // Pull the hour in 12 hour format
        hour_1 = (String)hourFormat12();
        
        if (hour_1.toInt() < 10)
        {
            hour_1 = "0" + hour_1;
        }

        min_1 = (String)minute();

        if (min_1.toInt() < 10)
        {
            min_1 = "0" + min_1;
        }

        if (weekday() == 1)
        {
            day_1 = "Sunday";
        }
        else if (weekday() == 2)
        {
            day_1 = "Monday";
        }
        else if (weekday() == 3)
        {
            day_1 = "Tuesday";
        }
        else if (weekday() == 4)
        {
            day_1 = "Wednesday";
        }
        else if (weekday() == 5)
        {
            day_1 = "Thursday";
        }
        else if (weekday() == 6)
        {
            day_1 = "Friday";
        }
        else if (weekday() == 7)
        {
            day_1 = "Saturday";
        }

        // Only print if the day stuff is not equal to each other
        if (day_1 != old_day_1 || hour_1 != old_hour_1 || min_1 != old_min_1 || am_pm_1 != old_am_pm_1 || date_ != old_date_ || print_time_state == true)
        {
            // Print black rectangle to prevent the am pm problem
            tft.fillRect(0,280,480, 320, BLACK);
            tft.setTextColor(ORANGE,BLACK);
            tft.setCursor(20, 280);
            tft.println(day_1 + " " + date_ + " " + hour_1 + ":" + min_1 + " " + am_pm_1);
            tft.setTextColor(WHITE, BLACK);
        }

        // Save the dates, so we can not print unless we change, so we can print a black box inbetween
        old_day_1 = day_1;
        old_hour_1 = hour_1;
        old_min_1 = min_1;
        old_am_pm_1 = am_pm_1;
        old_date_ = date_;
        print_time_state = false;
    }

    // Display main menu
    if (state == "1" && response != "") //--------------------------------------------------STATE 1 MAIN MENU--------------------------------------------------------//
    {
        // The grid position corresponds with the menu positions
        if (response == "top" && main_menu_grid > 0)
        {
            main_menu_grid = main_menu_grid - 1;
        }
        else if (response == "bottom" && main_menu_grid < 4)
        {
            main_menu_grid = main_menu_grid + 1;
        }

        // These if statements are responsible for highlighting menu items
        if (main_menu_grid == 0)
        {
            reset_main_menu();

            // Select this item
            tft.setTextColor(WHITE, GRAY);
            tft.setCursor(20, 80);
            tft.println("Set Feeding Times");
        }
        else if (main_menu_grid == 1)
        {
            reset_main_menu();

            // select this item
            tft.setTextColor(WHITE, GRAY);
            tft.setCursor(20, 120);
            tft.println("Set Clock");
        }
        else if (main_menu_grid == 2)
        {
            reset_main_menu();

            // select this item
            tft.setTextColor(WHITE, GRAY);
            tft.setCursor(20, 160);
            tft.println("Choose Calories Per Meal");
        }
        // else if (main_menu_grid == 3)
        // {
        //     reset_main_menu();

        //     // select this item
        //     tft.setTextColor(WHITE, GRAY);
        //     tft.setCursor(20, 200);
        //     tft.println("Give Calories Per Cup");
        // }
        else if (main_menu_grid == 3)
        {
            reset_main_menu();

            // select this item
            tft.setTextColor(WHITE, GRAY);
            tft.setCursor(20, 200);
            tft.println("Give Calories Per Gram");
        }
        else if (main_menu_grid == 4)
        {
            reset_main_menu();

            // select this item
            tft.setTextColor(WHITE, GRAY);
            tft.setCursor(20, 240);
            tft.println("Monitor Consumption");
        }



        // Add implementation for going to other menus/states
        if (main_menu_grid == 0 && response == "go")
        {
            // Go to the set feeding time menu
            state = "2";
            state_change = true;
        }
        // Change our state to the set clock state
        else if (main_menu_grid == 1 && response == "go")
        {
            
            state = "3";
            state_change = true;
        }
        // Change state to set calories per meal
        else if (main_menu_grid == 2 && response == "go")
        {  
            state = "4";
            state_change = true;
        }
        // Change state to be set calories per cup
        else if (main_menu_grid == 3 && response == "go")
        {
            state = "5";
            state_change = true;
        }
        // Set state to be calories per gram
        else if (main_menu_grid == 4 && response == "go")
        {
            state = "6";
            state_change = true;
        }
        // // Go to monitor consumption state
        // else if (main_menu_grid == 5 && response == "go")
        // {
        //     state = "7";
        //     state_change = true;
        // }

    }
    else if (state == "2" && response != "") //----------------------------------------------STATE 2 FEEDING TIME MENU-------------------------------------------------------//
    {
        // i is the number of items and j is the number of pages
        // Determine the number of items for the grid on our page
        max_item_bound = 0;
        for (int i = 0; i < 5; i++)
        {
            if (feeding_times[i][sft_page_number] != "")
            {
                max_item_bound++;
            }
        }


        // The grid position corresponds with the menu positions
        if (response == "top" && sft_vert_pos > 0)
        {
            sft_vert_pos--;
        }
        else if (response == "bottom" && sft_vert_pos < max_item_bound)
        {
            sft_vert_pos++;
        }
        else if (response == "left" && sft_horiz_pos > 0)
        {
            sft_horiz_pos--;
        }
        else if (response == "right" && sft_horiz_pos < 1)
        {
            sft_horiz_pos++;
        }
        // Go back to main menu
        else if (response == "back")
        {
            state = "1";
            state_change = true;
            sft_page_number = 0;
        }
        // Add a new time
        else if (sft_vert_pos == 0 && response == "go" && sft_page_number == 0)
        {
            state_change = true;
            state = "2a"; //--> GoTo add new feeding time
        }
        // Go to the next page if we're on page 0
        else if (sft_vert_pos == max_item_bound && sft_page_number == 0 && response == "go")
        {
            // We need to set max item bound to 0 to highlight right in this case
           // max_item_bound = 0;

            sft_page_number++;
            sft_vert_pos = 0;
            sft_horiz_pos = 0;
            sft_page_change = true;
            reset_feeding_time_menu();
            // Select this item
            tft.setTextColor(WHITE, GRAY);
            tft.setCursor(20, 280);
            tft.println("Previous Page");
        }
        // Go back 1 in the middle pages
        else if (sft_vert_pos == max_item_bound && sft_page_number != 0 && sft_page_number != 99 && response == "go" && sft_horiz_pos == 0)
        {
            sft_page_number--;
            sft_vert_pos = 0;
            sft_horiz_pos = 0;
            sft_page_change = true;
            reset_feeding_time_menu();
        }
        // Go forward 1 in the middle pages
        else if (sft_vert_pos == max_item_bound && sft_page_number != 0 && sft_page_number != 99 && response == "go" && sft_horiz_pos == 1)
        {
            sft_page_number++;
            sft_vert_pos = 0;
            sft_horiz_pos = 0;
            sft_page_change = true;
            reset_feeding_time_menu();
        }
        // Open menu to delete an existing time
        // Since this guy is at the bottom, it should bring up the menu in the right cases
        else if (response == "go")
        {
            // Pass the grid vertical position and the page number, then we can use that to get the array item
            Serial.println("10");
            state = "2b";
            state_change = true;
        }

        // Highlight the menu item that corresponds with our grid position
        if (state == "2" && sft_page_change == false)
        {
            if (sft_vert_pos == 0 && sft_horiz_pos == 0 && feeding_times[0][sft_page_number] != "")
            {
                reset_feeding_time_menu();

                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(20, 80);
                tft.println(feeding_times[0][sft_page_number]);
            }
            // Highlight first page "Next Page"
            else if (sft_vert_pos == max_item_bound && sft_page_number == 0)
            {
                // Reset the menu
                reset_feeding_time_menu();

                // Select this item
                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(20, 280);
                tft.println("Next Page");
            }
            // Highlight "Previous" page on all the middle pages
            else if (sft_vert_pos == max_item_bound && sft_horiz_pos == 0 && sft_page_number != 0 && sft_page_number != 99)
            {
                // Reset the menu
                reset_feeding_time_menu();

                // Select this item
                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(20, 280);
                tft.println("Previous Page");
                Serial.println("Here <==========================================");
                Serial.println("vert pos is: " + (String)sft_vert_pos);
                Serial.println("max item bound is: " + (String)max_item_bound);
                Serial.println("horiz pos is: " + (String)sft_horiz_pos);
                Serial.println("page num is: " + (String)sft_page_number);
            }
            // Highlight "Next page" on all the middle pages
            else if (sft_vert_pos == max_item_bound && sft_horiz_pos == 1 && sft_page_number != 0 && sft_page_number != 99)
            {
                // Reset the menu
                reset_feeding_time_menu();

                // Select this item
                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(300, 280);
                tft.println("Next Page");
            }
            // Highlight previous page on the last page
            else if (sft_vert_pos == max_item_bound && sft_horiz_pos == 1 && sft_page_number == 99)
            {
                // Reset the menu
                reset_feeding_time_menu();

                // Select this item
                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(300, 280);
                tft.println("Next Page");
            }
            else if (sft_vert_pos == 1 && sft_horiz_pos == 0)
            {
                reset_feeding_time_menu();

                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(20, 120);
                tft.println(feeding_times[1][sft_page_number]);
            }
            else if (sft_vert_pos == 2 && sft_horiz_pos == 0)
            {
                reset_feeding_time_menu();

                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(20, 160);
                tft.println(feeding_times[2][sft_page_number]);
            }
            else if (sft_vert_pos == 3 && sft_horiz_pos == 0)
            {
                reset_feeding_time_menu();

                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(20, 200);
                tft.println(feeding_times[3][sft_page_number]);
            }
            else if (sft_vert_pos == 4 && sft_horiz_pos == 0)
            {
                reset_feeding_time_menu();

                tft.setTextColor(WHITE, GRAY);
                tft.setCursor(20, 240);
                tft.println(feeding_times[4][sft_page_number]);
            }
        }
    }
    else if (state == "2a" && response != "") //------------------------------------------------STATE 2A SET NEW FEEDING TIME-----------------------------------------------------------------//
    {
        // Go back to page one of the set feeding time menu
        if (response == "back")
        {
            sft_page_number = 0;
            state_change = true;
            state = "2";
        }
        // Store the user's time in the feeding_times[][] array
        else if (response == "go")
        {
            // Determine what day the user input
            String day_str = "";
            switch (sft_day_num)
            {
            case 0:
                day_str = "Everyday";
                break;
            case 1:
                day_str = "Sunday";
                break;
            case 2:
                day_str = "Monday";
                break;
            case 3:
                day_str = "Tuesday";
                break;
            case 4:
                day_str = "Wednesday";
                break;
            case 5:
                day_str = "Thursday";
                break;
            case 6:
                day_str = "Friday";
                break;
            case 7:
                day_str = "Saturday";
                break;
            }
            // Add the user's time to the first entry in feeding_times that is not a blank string
            int i = 0;
            int j = 0;
            bool exit_i_loop = false;
            // Loop though the 10 pages (0 to 9)
            for (i = 0; i < 10; i++)
            {
                // Loop though the 5 page items (0 to 4)
                for (j = 0; j < 5; j++)
                {
                    // Break out of the outer loop 
                    if (exit_i_loop == true)
                    {
                        break;
                    }
                    if (feeding_times[j][i] == "")
                    {
                        // Factor in single digit minutes or hours
                        if (sft_hour < 10 && sft_minute < 10)
                        {
                            feeding_times[j][i] = day_str + " 0" + (String)sft_hour + ":0" + (String)sft_minute + " " + sft_am_pm;
                        }
                        else if (sft_hour < 10 && sft_minute > 9)
                        {
                            feeding_times[j][i] = day_str + " 0" + (String)sft_hour + ":" + (String)sft_minute + " " + sft_am_pm;
                        }
                        else if (sft_hour > 9 && sft_minute < 10)
                        {
                            feeding_times[j][i] = day_str + " " + (String)sft_hour + ":0" + (String)sft_minute + " " + sft_am_pm;
                        }
                        else
                        {
                            feeding_times[j][i] = day_str + " " + (String)sft_hour + ":" + (String)sft_minute + " " + sft_am_pm;
                        }
                        // Once we add the time to our array, exit to the set feeding times menu
                        sft_page_number = 0;
                        state_change = true;
                        state = "2";
                        exit_i_loop = true;
                        break;
                    }
                }
            }
        }
        // Update the screen with to match our changing grid position
        else if (sft_horiz_pos == 0 && response == "top" && sft_day_num < 7)
        {
            sft_day_num++;
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, "day");
        }
        else if (sft_horiz_pos == 0 && response == "bottom" && sft_day_num > 0)
        {
            sft_day_num--;
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, "day");
        }
        else if (sft_horiz_pos == 1 && response == "top" && (sft_hour < 12))
        {
            sft_hour++;
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, "hour");
        }
        else if (sft_horiz_pos == 1 && response == "bottom" && sft_hour > 1)
        {
            sft_hour--;
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, "hour");
        }
        else if (sft_horiz_pos == 2 && response == "top" && (sft_minute < 59))
        {
            sft_minute++;
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, "minute");
        }
        else if (sft_horiz_pos == 2 && response == "bottom" && sft_minute > 0)
        {
            sft_minute--;
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, "minute");
        }
        else if (sft_horiz_pos == 3 && response == "bottom" || sft_horiz_pos == 3 && response == "top")
        {
            if (sft_am_pm == "AM")
            {
                sft_am_pm = "PM";
            }
            else
            {
                sft_am_pm = "AM";
            }
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, "AM/PM");
        }
        else if (response == "right" && sft_horiz_pos < 3)
        {
            sft_horiz_pos++;
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, (String)sft_horiz_pos);
        }
        else if (response == "left" && sft_horiz_pos > 0)
        {
            sft_horiz_pos--;
            time_set_display(sft_day_num, sft_hour, sft_minute, sft_am_pm, (String)sft_horiz_pos);
        }
    }
    else if (state == "2b" && response != "") //------------------------------------------STATE 2B DELETE TIME--------------------------------------------------------------//
    {

        // Delete the item and resort the array to shift "" to the end
        if (response == "go")
        {
            // "delete" item
            feeding_times[sft_vert_pos][sft_page_number] = "";

            // Resort the array: Shift empty strings over towards the end
            bool sorted = false;

            // Increase sort check when we find the first ""
            // After that increase it if we find an element that is not ""
            // If sort_check is 2 exit check and do another pass of sorting
            int sort_check = 0;
            
            while (sorted == false)
            {
                // Loop though the pages
                for (int i = 0; i < 10; i++)
                {
                    // Go though the page items
                    for (int j = 0; j < 5; j++)
                    {
                        if (feeding_times[j][i] == "")
                        {
                            // If we have "" swap with the next item
                            if (j != 4)
                            {
                                feeding_times[j][i] = feeding_times[j + 1][i];
                                feeding_times[j + 1][i] = "";
                            }
                            // Swap with the next page item, but not the end
                            else if (j == 4 && i != 9)
                            {
                                feeding_times[j][i] = feeding_times[0][i + 1];
                                feeding_times[0][i + 1] = "";
                            }
                        }
                    }
                }

                // Loop though everything to check if it's sorted lol, goofiest sorting ever
                for (int i = 0; i < 10; i++)
                {
                    // Go though the page items
                    for (int j = 0; j < 5; j++)
                    {
                        if (feeding_times[j][i] == "" && sort_check == 0)
                        {
                            sort_check++;
                        }
                        else if (sort_check == 1 && feeding_times[j][i] != "")
                        {
                            sort_check++;
                            break;
                        }
                    }
                    if (sort_check == 2)
                    {
                        break;
                    }
                }
                // Check if we got though successfully
                if (sort_check != 2)
                {
                    sorted = true;
                }
                // Reset sort_check back to 0
                sort_check = 0;
            }
        }

        // If they do delete the time, or don't want to delete the time, we go back to feeding time menu
        if (response == "go" || response == "back")
        {
            state = "2";
            state_change = true;
        }

    }
    else if (state == "3" && response != "")    //-----------------------------------------STATE 3 SET CLOCK-------------------------------------------------------//
    {
        // Pause the program and set the clock
        set_clock(false);
        // Go back to the main menu 
        state = "1";
        state_change = true;
        // Turn this back off just in case
        wait_one_min = false;
    }
    else if (state == "4" && response != "")    //-----------------------------------------STATE 4 GIVE CALORIES PER Meal-----------------------------------------------//
    {
        if (response == "back")
        {
            state_change = true;
            state = "1";
            // Use the prevous value not the changing value
            cal_per_meal = old_cal_per_meal;
        }
        else if (response == "go")
        {
            state_change = true;
            state = "1";
        }
        else if (response == "top")
        {
            cal_per_meal = cal_per_meal + 5;
        }
        else if (response == "bottom")
        {
            cal_per_meal = cal_per_meal - 5;;
        }

        if (cal_per_meal <= 0)
        {
            cal_per_meal = 0;
        }

        // Print the updated calories per meal
        tft.setTextColor(WHITE, GRAY);
        tft.setTextSize(4);
        tft.setCursor(180, 200);
        tft.fillRect(160, 160, 280, 260, BLACK);
        tft.print(cal_per_meal);
    }
    else if (state == "5" && response != "")    //-----------------------------------------STATE 5 GIVE CALORIES PER GRAM-----------------------------------------------//
    {
        if (response == "back")
        {
            state_change = true;
            state = "1";
            cal_per_gram = old_cal_per_gram;
        }
        else if (response == "go")
        {
            state_change = true;
            state = "1";
        }
        else if (response == "top")
        {
            cal_per_gram = cal_per_gram + 0.25;
        }
        else if (response == "bottom")
        {
            cal_per_gram = cal_per_gram - 0.25;;
        }

        if (cal_per_gram <= 0)
        {
            cal_per_gram = 0;
        }

        tft.setTextColor(WHITE, GRAY);
        tft.setTextSize(4);
        tft.setCursor(195, 200);
        tft.fillRect(150, 160, 280, 260, BLACK);
        tft.print(cal_per_gram);
    }
    else if (state == "6" && response != "")    //----------------------------------------STATE 6 MONITOR CONSUMPTION-------------------------------------------------//
    {
        if (response == "back")
        {
            state_change = true;
            state = "1";
        }
        
    }   //--------------------------------------------------------END OF STATES------------------------------------------------------------//

    // Reset buton input to a blank string
    response = "";

}

//===========================================================================================
//                            Create a method for each menu option
//===========================================================================================

// Each menu will have integers to act as a grid system
// The current grid items will be highlighted gray

// Ask the user what way they want to enter calories in on startup
String calorie_method()
{
    tft.fillRect(0, 0, 480, 320, BLACK);
    tft.setTextSize(4);

    tft.setCursor(20, 80);
    tft.setTextColor(GREEN, BLACK);
    tft.println("(GO)Enter calories");
    tft.setCursor(20, 120);
    tft.println("in grams");
    tft.setCursor(20, 200);
    tft.setTextColor(RED, BLACK);
    tft.println("(BACK)Enter");
    tft.setCursor(20, 240);
    tft.println("calories per cup");

    String yes_no = "";
    // Stop and read in user input, we can stop here since we are not in the loop yet
    while (1)
    {
        yes_no = read_buttons();
        if (yes_no == "go" || yes_no == "back")
        {
            return yes_no;
        }
    }
}


// For each menu we need to un highlight text, however you can't do that
// Instead of undoing the white highlight you must add a black highlight
// Each menu may need to have a method that resets the menu before highlighting
void reset_main_menu()
{
    tft.setTextWrap(true);
    tft.setCursor(20, 80);
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(3);
    tft.println("Set Feeding Times");
    tft.setCursor(20, 120);
    tft.println("Set Clock");
    tft.setCursor(20, 160);
    tft.println("Choose Calories Per Meal");
    // tft.setCursor(20, 200);
    // tft.println("Give Calories Per Cup");
    tft.setCursor(20, 200);
    tft.println("Give Calories Per Gram");
    tft.setCursor(20, 240);
    tft.println("Monitor Consumption");

}



// Prints the feeding time menu with black highlighting on each entry
int reset_feeding_time_menu()
{
    // Display the header
    tft.setCursor(40, 20);
    tft.setTextSize(4);
    tft.setTextColor(ORANGE, BLACK);
    tft.drawRect(20, 10, 440, 50, WHITE);
    tft.println("Set Feeding Times");
    tft.setTextSize(3);
    tft.setTextColor(WHITE);

    // We want to clear everything, but the header when we change pages
    if (sft_page_change == true)
    {
        sft_page_change = false;
        tft.fillRect(0, 70, 480, 320, BLACK);
    }

    // This prints the all of the feeding times again without highlighting
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(20, 80);
    tft.println(feeding_times[0][sft_page_number]);
    tft.setCursor(20, 120);
    tft.println(feeding_times[1][sft_page_number]);
    tft.setCursor(20, 160);
    tft.println(feeding_times[2][sft_page_number]);
    tft.setCursor(20, 200);
    tft.println(feeding_times[3][sft_page_number]);
    tft.setCursor(20, 240);
    tft.println(feeding_times[4][sft_page_number]);

    // On every page execpt page 1 and 100 we print both
    if (sft_page_number != 0 && sft_page_number != 99)
    {
        tft.setCursor(20, 280);
        tft.println("Previous Page");
        tft.setCursor(300, 280);
        tft.println("Next Page");
    }
    // On page 1 only have next page
    else if (sft_page_number == 0)
    {
        // put the previous page up front
        tft.setCursor(20, 280);
        tft.println("Next Page");
    }
    else if (sft_page_number == 99)
    {
        // put the previous page up front
        tft.setCursor(20, 280);
        tft.println("Previous Page");
    }

    // if (max_item_bound == 0 && sft_page_number != 0 && sft_page_change == true)
    // {
    //     // Select the previous page if we have no other items on a page change
    //     tft.setTextColor(WHITE, GRAY);
    //     tft.setCursor(20, 280);
    //     tft.println("Previous Page");
    //     Serial.println("=====================> Here!");
    //     delay(3000);
    // }

}

// Delete the time if the user wants to
// Use the page_number and vertical position to find the array position
void delete_item()
{
    tft.fillRect(0, 0, 480, 320, BLACK);
    tft.setTextSize(4);

    tft.setCursor(20, 100);
    tft.setTextColor(GREEN, BLACK);
    tft.println("Press GO to delete");
    tft.setCursor(20, 200);
    tft.setTextColor(RED, BLACK);
    tft.println("Press BACK to exit");

    // Stop and read in user input
    String response = "";
    while (1)
    {
        response = read_buttons();
        if (response == "go" || response == "back")
        {
            break;
        }
    }

    // Delete the item and resort the array to shift "" to the end
    if (response == "go")
    {
        // "delete" item
        feeding_times[sft_vert_pos][sft_page_number] = "";

        // Resort the array: Shift empty strings over towards the end

        String temp = "";
        bool sorted = false;
        
        // Increase sort check when we find the first ""
        // After that increase it if we find an element that is not ""
        // If sort_check is 2 exit check and do another pass of sorting
        int sort_check = 0;

        while (sorted == false)
        {
            // Loop though the pages
            for (int i = 0; i < 10; i++)
            {
                // Go though the page items
                for (int j = 0; j < 5; j++)
                {
                    if (feeding_times[j][i] == "")
                    {
                        // If we have "" swap with the next item
                        if (j != 4)
                        {
                            feeding_times[j][i] = feeding_times[j+1][i];
                            feeding_times[j+1][i] = "";
                        }
                        // Swap with the next page item, but not the end
                        else if (j == 4 && i != 9)
                        {
                            feeding_times[j][i] = feeding_times[0][i+1];
                            feeding_times[0][i+1] = "";
                        }
                    }
                }
            }

            // Loop though everything to check if it's sorted lol, goofiest sorting ever
            for (int i = 0; i < 10; i++)
            {
                // Go though the page items
                for (int j = 0; j < 5; j++)
                {
                    if (feeding_times[j][i] == "" && sort_check == 0)
                    {
                        sort_check++;
                    }
                    else if (sort_check == 1 && feeding_times[j][i] != "")
                    {
                        sort_check++;
                        break;
                    }
                }
                if (sort_check == 2)
                {
                    break;
                }
            }
            // Check if we got though successfully
            if (sort_check != 2)
            {
                sorted = true;
            }
        }


    }

    // Set some of the menu stuff back to normal
    tft.fillRect(0, 0, 480, 320, BLACK);
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(3);
}


// Method for displaying changes in the time set menu depending on the grid position
void time_set_display(int day_num, int hour, int minute, String am_pm, String change_item)
{
    // Delay because we are moving between options way too quickly
    delay(150);
    // Change the day depending on our grid position for the grid (day_num)
    String day_str = "";
    switch (day_num)
    {
    case 0:
        day_str = "Everyday";
        break;
    case 1:
        day_str = "Sunday";
        break;
    case 2:
        day_str = "Monday";
        break;
    case 3:
        day_str = "Tuesday";
        break;
    case 4:
        day_str = "Wednesday";
        break;
    case 5:
        day_str = "Thursday";
        break;
    case 6:
        day_str = "Friday";
        break;
    case 7:
        day_str = "Saturday";
        break;
    }

    // Highlight the current grid position gray and highlight the adjacent grid positions black
    tft.setTextColor(WHITE, GRAY);
    if (change_item == "day")
    {
        tft.fillRect(80, 150, 175, 40, BLACK);
        tft.setCursor(80, 150);
        tft.print(day_str);

        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(260, 150);
        if (hour < 10)
        {
            tft.print("0" + (String)hour);
        }
        else
        {
            tft.print((String)hour);
        }
    }
    else if (change_item == "hour")
    {
        tft.setCursor(260, 150);
        if (hour < 10)
        {
            tft.print("0" + (String)hour);
        }
        else
        {
            tft.print((String)hour);
        }

        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(80, 150);
        tft.print(day_str);
        tft.setCursor(310, 150);
        if (minute < 10)
        {
            tft.print("0" + (String)minute);
        }
        else
        {
            tft.print((String)minute);
        }
    }
    else if (change_item == "minute")
    {
        tft.setCursor(310, 150);
        if (minute < 10)
        {
            tft.print("0" + (String)minute);
        }
        else
        {
            tft.print((String)minute);
        }
    }
    else if (change_item == "AM/PM")
    {
        tft.setCursor(350, 150);
        tft.print(am_pm);
    }
    else if (change_item == "0")
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(80, 150);
        tft.print(day_str);

        // Print adjacent ones black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(260, 150);
        if (hour < 10)
        {
            tft.print("0" + (String)hour);
        }
        else
        {
            tft.print((String)hour);
        }
    }
    else if (change_item == "1")
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(260, 150);
        if (hour < 10)
        {
            tft.print("0" + (String)hour);
        }
        else
        {
            tft.print((String)hour);
        }

        // Print adjacent ones black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(80, 150);
        tft.print(day_str);

        tft.setCursor(310, 150);
        if (minute < 10)
        {
            tft.print("0" + (String)minute);
        }
        else
        {
            tft.print((String)minute);
        }
    }
    else if (change_item == "2")
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(310, 150);
        if (minute < 10)
        {
            tft.print("0" + (String)minute);
        }
        else
        {
            tft.print((String)minute);
        }

        // Print adjacent ones black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(260, 150);
        if (hour < 10)
        {
            tft.print("0" + (String)hour);
        }
        else
        {
            tft.print((String)hour);
        }

        tft.setCursor(350, 150);
        tft.print(am_pm);
    }
    else if (change_item == "3")
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(350, 150);
        tft.print(am_pm);

        // Print ajacent ones black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(310, 150);
        if (minute < 10)
        {
            tft.print("0" + (String)minute);
        }
        else
        {
            tft.print((String)minute);
        }
    }
}

// Create method for the user to set the time
void set_clock(bool force_selection)
{
    // Need to get the hour, minute, day, month, and year
    // An underscore is added on the end of these names because the time arduino lib uses those names for functions
    int month_ = 1;
    int day_ = 1;
    int year_ = 2021;
    int hour_ = 2;
    int minute_ = 0;
    String am_pm = "AM";

    // 0--> month, 1--> day, 2--> year, 3--> hour, 4--> minute, 5--> AM/PM
    // Reset response to an empty string here or we zoom thru the menu
    response = "";
    int horiz_pos = 0;
    while (1)
    {
        response = read_buttons();
        if (response == "back" && force_selection == false)
        {
            state = "1";
            state_change = true;
            return;
        }
        else if (response == "go") 
        {
            if (am_pm == "PM")
            {
                hour_ = hour_ + 12;
            }

            // Set the clock
            setTime(hour_, minute_, 0, day_, month_, year_);

            // Print the time out to the serial monitor
            current_time = now();

            // Go back to the main menu after setting the clock
            state = "1";
            state_change = true;

            // Return to exit the fucntion
            return;
        }
        else if (horiz_pos == 0 && response == "top" && month_ < 12)
        {
            month_++;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "month");
        }
        else if (horiz_pos == 0 && response == "bottom" && month_ > 1)
        {
            month_--;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "month");
        }
        // Later when setting the date, if the user goes over just set their day down to 30
        else if (horiz_pos == 1 && response == "top" && day_ < 31)
        {
            day_++;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "day");
        }
        else if (horiz_pos == 1 && response == "bottom" && day_ > 1)
        {
            day_--;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "day");
        }
        else if (horiz_pos == 2 && response == "top" && year_ < 2050)
        {
            year_++;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "year");
        }
        else if (horiz_pos == 2 && response == "bottom" && year_ > 2000)
        {
            year_--;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "year");
        }
        else if (horiz_pos == 3 && response == "top" && (hour_ < 12))
        {
            hour_++;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "hour");
        }
        else if (horiz_pos == 3 && response == "bottom" && hour_ > 1)
        {
            hour_--;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "hour");
        }
        else if (horiz_pos == 4 && response == "top" && (minute_ < 59))
        {
            minute_++;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "minute");
        }
        else if (horiz_pos == 4 && response == "bottom" && minute_ > 0)
        {
            minute_--;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "minute");
        }
        // Make a change for am/pm
        else if (horiz_pos == 5 && response == "bottom" || horiz_pos == 5 && response == "top")
        {
            if (am_pm == "AM")
            {
                am_pm = "PM";
            }
            else
            {
                am_pm = "AM";
            }
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, "AM/PM");
        }
        else if (response == "right" && horiz_pos < 5)
        {
            horiz_pos++;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, (String)horiz_pos);
        }
        else if (response == "left" && horiz_pos > 0)
        {
            horiz_pos--;
            clock_set_display(month_, day_, year_, hour_, minute_, am_pm, (String)horiz_pos);
        }

        // Delay a bit because it goes so fast it's hard to select a time
        delay(100);
        // Clear the response, so we don't keep moving 
        response = "";
    }
}

// Method for displaying changes in the clock set menu
void clock_set_display(int month, int day, int year, int hour, int minute, String am_pm, String change_item)
{
    // Make the background black so we paint over old stuff

    tft.setTextColor(WHITE, GRAY);
    if (change_item == "month")
    {
        tft.setCursor(70, 175);
        if (month < 10)
        {
            tft.println("0" + (String)month);
        }
        else
        {
            tft.println((String)month);
        }
    }
    else if (change_item == "day")
    {
        tft.setCursor(125, 175);
        if (day < 10)
        {
            tft.println("0" + (String)day);
        }
        else
        {
            tft.println((String)day);
        }
    }
    else if (change_item == "year")
    {
        tft.setCursor(180, 175);
        tft.println((String)year);
    }
    else if (change_item == "hour")
    {
        tft.setCursor(280, 175);
        if (hour < 10)
        {
            tft.println("0" + (String)hour);
        }
        else
        {

            tft.println((String)hour);
        }
    }
    else if (change_item == "minute")
    {
        if (minute < 10)
        {
            tft.setCursor(330, 175);
            tft.println("0" + (String)minute);
        }
        else
        {
            tft.setCursor(330, 175);
            tft.println((String)minute);
        }
    }
    else if (change_item == "AM/PM")
    {
        tft.setCursor(375, 175);
        tft.println(am_pm);
    }
    // Now have position ones to highlight when going fight and left
    else if (change_item == "0") // month
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(70, 175);
        if (month < 10)
        {
            tft.println("0" + (String)month);
        }
        else
        {
            tft.println((String)month);
        }
        // Make adjacent items black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(125, 175);
        if (day < 10)
        {
            tft.println("0" + (String)day);
        }
        else
        {
            tft.println((String)day);
        }
    }
    else if (change_item == "1") // day
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(125, 175);
        if (day < 10)
        {
            tft.println("0" + (String)day);
        }
        else
        {
            tft.println((String)day);
        }

        // Make others black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(70, 175);
        if (month < 10)
        {
            tft.println("0" + (String)month);
        }
        else
        {
            tft.println((String)month);
        }
        tft.setCursor(180, 175);
        tft.println((String)year);
    }
    else if (change_item == "2") // Year
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(180, 175);
        tft.println((String)year);

        // Make near black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(125, 175);
        if (day < 10)
        {
            tft.println("0" + (String)day);
        }
        else
        {
            tft.println((String)day);
        }
        tft.setCursor(280, 175);
        if (hour < 10)
        {
            tft.println("0" + (String)hour);
        }
        else
        {
            tft.println(hour);
        }
    }
    else if (change_item == "3") // hour
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(280, 175);
        if (hour < 10)
        {
            tft.println("0" + (String)hour);
        }
        else
        {
            tft.println(hour);
        }

        // Make near black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(330, 175);
        if (minute < 10)
        {
            tft.println("0" + (String)minute);
        }
        else
        {
            tft.println(minute);
        }
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(180, 175);
        tft.println((String)year);
    }
    else if (change_item == "4") // minute
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(330, 175);
        if (minute < 10)
        {
            tft.println("0" + (String)minute);
        }
        else
        {
            tft.println(minute);
        }

        // Makes others black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(280, 175);
        if (hour < 10)
        {
            tft.println("0" + (String)hour);
        }
        else
        {
            tft.println(hour);
        }
        tft.setCursor(375, 175);
        tft.println(am_pm);
    }
    else if (change_item == "5") // AM/PM
    {
        tft.setTextColor(WHITE, GRAY);
        tft.setCursor(375, 175);
        tft.println(am_pm);

        // Set adjacent black
        tft.setTextColor(WHITE, BLACK);
        tft.setCursor(330, 175);
        if (minute < 10)
        {
            tft.println("0" + (String)minute);
        }
        else
        {
            tft.println(minute);
        }
    }

    // Set the text color back to just white
    tft.setTextColor(WHITE, BLACK);
}

//int month, int day, int year, int hour, int minute, String am_pm, String change_item)
void set_clock_print_all_black(int month, String day, int year, int hour, int minute, String am_pm)
{
    // Change text to black
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(70, 150);
    if (month < 10)
    {
        tft.println("0" + (String)month);
    }
    else
    {
        tft.println((String)month);
    }
    tft.setCursor(125, 150);
    if (day < 10)
    {
        tft.println("0" + (String)day);
    }
    else
    {
        tft.println((String)day);
    }
    tft.setCursor(180, 150);
    tft.println((String)year);
}

// Allow user to set the number of calories per cup
void set_calories()
{
}

// Funtion to dispense food when called
void dispense_food()
{
    // String response = "";

    // tft.fillRect(0, 0, 480, 320, BLUE);
    // tft.fillRect(5, 5, 470, 310, BLACK);
    // tft.setCursor(80, 150);
    // tft.setTextColor(WHITE);
    // tft.println("TESTING MOTOR");

    // long start_time = millis();

    // while (response != "back")
    // {
    //     for(int i = 0; i < 5; i++)
    //     {
    //         Serial.println("Motor Going");
    //         digitalWrite(motorPin4, LOW);
    //         digitalWrite(motorPin3, LOW);
    //         Serial.println("STOP!!!");
    //         delay(1000);
    //         Serial.println("Goooooooo");
    //         digitalWrite(motorPin4, LOW);
    //         digitalWrite(motorPin3, HIGH);
    //         analogWrite(speedPin1, 255);
    //         delay(80);
    //     }
    //     response = read_buttons();

    // }
    // Serial.println( (String)(millis() - start_time));
    // digitalWrite(motorPin4, LOW);
    // digitalWrite(motorPin3, LOW);

    // // Return to main menu when we press back
    // main_menu();
}

//   Serial.print("Reading: ");
//   Serial.print((scale.get_units()) * -453.592, 2); //scale.get_units() returns a float
//   Serial.print(" grams"); //You can change this to kg but you'll need to refactor the calibration_factor

// Read the weight in the load cell to tell the owner how much food the pet is eating
// We will also use the reading to not dispense any food if it would cause the bowl to overflow
void read_load_cell()
{
    // tft.fillRect(0, 0, 480, 320, BLUE);
    // tft.fillRect(5, 5, 470, 310, BLACK);
    // tft.setTextColor(WHITE,BLACK);
    // tft.setCursor(80, 150);

    // String response = "";
    // float reading = 0; //, 2);
    // while (response != "back")
    // {
    //     reading = scale.get_units() * -453.592;
    //     tft.setCursor(80, 150);
    //     tft.print("Reading: " + (String)reading + " grams");
    //     delay(100);
    //     response = read_buttons_load_cell();
    //     Serial.println("Goin");
    // }
    // tft.setTextColor(WHITE);
    // main_menu();
}


// Function: read in button input. Don't stop the program while reading

String read_buttons()
{
    // The pinmodes for the buttons are set to INPUT_PULLUP because the reading can be ambigous otherwise
    // Arduino uses negative logic, so the states are opposite of what you would think--> LOW means pressed button
    long start_time = millis();
    //response = "";
    while(millis() - start_time <= 200)
    {
        if (digitalRead(left_button) == LOW)
        {
            Serial.println("left");
            return "left";
        }
        else if (digitalRead(bottom_button) == LOW)
        {
            Serial.println("bottom");
            return "bottom";
        }
        else if (digitalRead(right_button) == LOW)
        {
            Serial.println("right");
            return "right";
        }
        else if (digitalRead(top_button) == LOW)
        {
            Serial.println("top");
            return "top";
        }
        else if (digitalRead(go_button) == LOW)
        {
            Serial.println("go");
            return "go";
        }
        else if (digitalRead(back_button) == LOW)
        {
            Serial.println("back");
            return "back";
        }
    }
    // We will also want a delay so holding the button down doesn't give crazy results
    delay(100);
    return response;
}

// Function: Acts like a split string to seperate strings into smaller strings
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
