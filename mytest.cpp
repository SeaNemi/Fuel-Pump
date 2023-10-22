/***************************
 ** File:    mytest.cpp
 ** Project: CMSC 341, proj0, Spring 2023
 ** Author:  William Whatley
 ** Date:    2/23/2023
 ** Section: Marron
 ** Email:   w59@umbc.edu
 **
 ** This file is written as a test for fuel.cpp. Numerous cases are tested to ensure
 ** that the code works properly. Normal cases, edge cases, and error cases are tested, alongside
 ** other tests such as testing the drain function, add and remove Tank, and the addition and removal of pumps.
 **
*****************************/

#include "fuel.h"
#include <random>

//two constant integers are declared and initialized
//used specifically for setTest, which set the number of tanks and pumps
const int PURE_EDGE_CASE = 0; //PURE_EDGE_CASE is 0 since MAX_INT always created in setTest
const int ID_EDGE_CASE = 1; //ID Edge Case contains both 0 tankID with a pump and MAX_INT
const int NORMAL_CASE = 10; //normal case contains 11 tanks, ten with normal IDs and one MAX_INT ID
const int MAX_INT = 2147483647; //constant for a max integer, which is the highest possible case

//Random class brought over from driver file and used for determining tanks
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else{ //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution
};

class Tester{
    public:
        void testCondition(bool var){
            if (var){
                cout << "This test has passed" << endl << endl;
            }
            else{
                cout << "This test has failed" << endl << endl;
            }
        }

        //setTest
        //Sets the tanks and pumps for a normal test
        void setTest(FuelSys& sys, int numTanks){
            
            //randFuel and randTank declared and constructued, used for generating random tankCaps and pump target tanks
            Random randFuel(MINCAP,DEFCAP);
            Random randTank(0,(numTanks - 1));

            for (int i = 0; i < numTanks; i++){
                sys.addTank(i, DEFCAP, randFuel.getRandNum());
            }
            //edge case ID added for further testing
            sys.addTank(MAX_INT, DEFCAP, randFuel.getRandNum());

            //pumps added which have a target for each tank
            for (int i = 0 ;i < numTanks; i++){
                for (int j = 0; j < numTanks; j++){
                    //random number created
                    int randNum = randTank.getRandNum();
                    //ternary operator used to ensure that the targetTank does not equal the tankID so it doesn't pump to self
                    int targetNum = (randNum == i) ? MAX_INT : randNum; //MAX_INT used as a placeholder
                    sys.addPump(i, j, targetNum);
                }
            }  
        }

        //findPump
        //function which checks if the pump is within the list
        bool findPump(Tank& tank, int pumpID){
            
            //if statement checks to ensure m_pumps is not empty
            if (tank.m_pumps == nullptr){
                return false;
            }
            //else pumpCurr traverses through the singly linked list
            else{
                Pump* pumpCurr = tank.m_pumps; //pumpCurr declared and initialized to m_pumps

                //do while goes through the singly linked list to ensure that a pump with that ID exists
                do{
                    //if statement checks if the pumpID equals m_pumpID
                    if (pumpCurr->m_pumpID == pumpID){
                        return true;
                    }
                    pumpCurr = pumpCurr->m_next;
                } while (pumpCurr != nullptr);
                return false;
            }
        }

        //emptyTankAddition
        //adds empty tank objects to the system object
        void emptyTankAddition(FuelSys& sys, int numTanks){
            //for loop goes through and adds pump-less tanks to the list
            for (int i = 0; i < numTanks; i++){
                sys.addTank(i, DEFCAP, 0);
            }
            //edge case tested for pumpID with an ID of the max int
            sys.addTank(MAX_INT, DEFCAP, 0);
        }

        //testAddTank
        //Numerous tanks are added to the function
        bool testAddTank(FuelSys& sys){
            bool result = true;
            
            //randFuel is declared and constructed with a fuel capacity between MINCAP (2000) and DEFCAP (5000)
            Random randFuel(MINCAP,DEFCAP);
            
            //int HIGHCAP declared and initialized to 1,000,000
            int HIGHCAP = 1000000;
            //highFuelCap declared and constructed between MINCAP and HIGHCAP
            Random highFuelCap(MINCAP, HIGHCAP);

            //result tests an edge case, which is one with an ID of 0 and a minimum capacity
            result = result && sys.addTank(0, MINCAP, MINCAP);
            result = result && (sys.m_current->m_tankID == 0);

            //tests addTank with small fuel capacities
            result = result && sys.addTank(1, MINCAP, MINCAP);
            result = result && (sys.m_current->m_tankID == 1);
            result = result && sys.addTank(2, MINCAP, MINCAP);
            result = result && (sys.m_current->m_tankID == 2);

            //tests addTank with higher IDs and checks to make sure m_current is the last added tank
            result = result && sys.addTank(100, DEFCAP, randFuel.getRandNum());
            result = result && (sys.m_current->m_tankID == 100);
            result = result && sys.addTank(1000, DEFCAP, randFuel.getRandNum());
            result = result && (sys.m_current->m_tankID == 1000);
            result = result && sys.addTank(10000, DEFCAP, randFuel.getRandNum());
            result = result && (sys.m_current->m_tankID == 10000);

            //tests addTank with higher tank capacity
            for (int i = 10; i < 20; i++){
                result = result && sys.addTank(i, HIGHCAP, highFuelCap.getRandNum());
                result = result && (sys.m_current->m_tankID == i); //checks to make sure last added is m_current
            }


            //edge case where m_tankID and m_tankCapacity is the highest value an integer can be in C++
            //m_tankFuel NOT set to MAX_INT due to potential to mess up totalFuel and have an errored object
            result = result && sys.addTank(MAX_INT, MAX_INT, 0);
            result = result && (sys.m_current->m_tankID == MAX_INT);

            //removes a tank, then readds it and checks if it is in the list
            result = result && sys.removeTank(0);
            result = result && sys.addTank(0, DEFCAP, DEFCAP);
            result = result && (sys.m_current->m_tankID == 0);

            return result; //result is returned
        }
        //errorAddTank
        //Attempts to add a bunch of errant tanks to the linked list
        bool errorAddTank(FuelSys& sys){
            bool result = true;

            //result checks that m_current has an ID of 0
            result = result && (sys.m_current->m_tankID == 0);

            //attempts to add the same tank
            result = result && !(sys.addTank(0, DEFCAP, 80));

            //zero tank removed
            result = result && sys.removeTank(0);

            //attempts to add tanks with errant IDs
            result = result && !(sys.addTank(-1, DEFCAP, 0));
            result = result && !(sys.addTank(-1.07, DEFCAP, 3000));

            //attempts to add tanks with errant tankCaps
            result = result && !(sys.addTank(20, 1000, 0));
            result = result && !(sys.addTank(4.07, 10, 200));
            result = result && !(sys.addTank(30, 1999.9, 0));
            result = result && !(sys.addTank(20, -30, MINCAP));

            //attempts to add tanks with errant tankFuel
            result = result && !(sys.addTank(290, DEFCAP, -1));
            result = result && !(sys.addTank(50, DEFCAP, -123.3));

            //attempts to add repeated tanks, one normal, one with an errant tankCap, and one with an errant tankFuel
            result = result && !(sys.addTank(1, -1999.9, MINCAP));
            result = result && !(sys.addTank(1, DEFCAP, -200));
            result = result && !(sys.addTank(1, -1999.9, -1));

            //final test to ensure that m_current is nullptr so nothing was added
            result = result && (sys.m_current == nullptr);

            //result is returned
            return result;
        }

        //testRemoveTank
        //attempts to remove tanks within the system
        bool testRemoveTank(FuelSys &sys, int numTanks){
            bool result = true;

            //attempts to remove tanks in a for loop
            for (int i = 0; i < numTanks; i++){

                //tank is found prior to ensure the pointer past the one being removed is accounted for
                sys.findTank(i);
                Tank* nextTank = sys.m_current->m_next->m_next;
                result = result && sys.removeTank(i);

                //result checks to make sure that m_current->m_next is equal to the pointer after the one deleted
                result = result && (sys.m_current->m_next->m_tankID == nextTank->m_tankID);

                //checks to make sure m_current is not nullptr yet
                result = result && (sys.m_current != nullptr);
            }

            //removes the edge case with the highest possible ID
            result = result && sys.removeTank(MAX_INT);

            //checks to ensure that m_current is equal to nullptr and everything is removed
            result = result && (sys.m_current == nullptr);

            //uses setTest tanks again to test that they work
            setTest(sys, numTanks);

            //removes tanks in reverse order
            for (int i = (numTanks - 1); i >= 0; --i){
                //tank is found prior to ensure the pointer past the one being removed is accounted for
                sys.findTank(i);
                Tank* nextTank = sys.m_current->m_next->m_next;
                result = result && sys.removeTank(i);

                //result checks to make sure that m_current->m_next is equal to the pointer after the one deleted
                result = result && (sys.m_current->m_next->m_tankID == nextTank->m_tankID);

                //checks to make sure m_current is not nullptr yet
                result = result && (sys.m_current != nullptr);
            }
            //removes the edge case with the highest possible ID
            result = result && sys.removeTank(MAX_INT);

            //checks to ensure that m_current is equal to nullptr and everything is removed
            result = result && (sys.m_current == nullptr);

            return result;
        }

        //error removeTank
        //tests all error cases for remove tanks and ensure that it works properly
        bool errorRemoveTank(FuelSys& sys, int numTanks){
            bool result = true;

            //removes a tank normally, then tries to remove it again
            result = result && sys.removeTank(MAX_INT);
            result = result && !(sys.removeTank(MAX_INT));

            //attempts to remove tanks which have negative numbers
            result = result && !(sys.removeTank(-70));
            result = result && !(sys.removeTank(-1.01));
            result = result && !(sys.removeTank(-10000));
            result = result && !(sys.removeTank(-50));

            //attempts to remove tanks with the negative IDs of tanks added
            //starts at 1, since -0 is 0, which is still in the list
            for (int i = 1; i < numTanks; i++){
                result = result && !(sys.removeTank(-i));
            }

            //attempts to remove tanks which do not exist in the list based on normalTestSet
            result = result && !(sys.removeTank(30));
            result = result && !(sys.removeTank(2000));

            result = result && !(sys.removeTank(10));
            result = result && !(sys.removeTank(378928));

            //attempts to add an error case, then tries to remove said tank, even though it should not be added
            result = result && !(sys.addTank(80, 1999, MINCAP));
            result = result && !(sys.removeTank(80));

            return result; //result is returned
        }

        //testFindTank
        //attempts to find tanks within the list
        bool testFindTank(FuelSys& sys, int numTanks){
            bool result = true;

            //for loop goes through the linked list and finds all tanks which were added
            for (int i = 0; i < numTanks; i++){
                result = result && sys.findTank(i); //checks to ensure the tank is found
                result = result && (sys.m_current->m_next->m_tankID == i); //checks to ensure that m_next of current is the found tank
            }

            //attempts to find a tank with the highest possible ID
            result = result && sys.findTank(MAX_INT);
            result = result && (sys.m_current->m_next->m_tankID == MAX_INT);
            return result;
        }

        //errorFindTank
        //attempts to perform error cases on findTank
        bool errorFindTank(FuelSys& sys, int numTanks){
            bool result = true;

            //removes a tank from the list, then attempts to find it
            result = result && sys.removeTank(MAX_INT);
            result = result && !(sys.findTank(MAX_INT));
            
            //attempts to find tanks with negative numbers
            result = result && !(sys.findTank(-100));
            result = result && !(sys.findTank(-1.20));
            result = result && !(sys.findTank(-70));
        
            //attempts to find tanks with the negative IDs of tanks added
            //starts at 1, since -0 equals 0
            for (int i = 1; i < numTanks; i++){
                result = result && !(sys.findTank(-i));
            }

            //attempts to add an error case, then tries to find said tank, even though it should not be added
            result = result && !(sys.addTank(80, 1999, 0));
            result = result && !(sys.findTank(80));

            return result;
        }

        //testTotalFuel
        //tests total fuel for normal cases
        bool testTotalFuel(FuelSys& sys){
            bool result = true;
            int fuelAdded = 0;

            //for loop add tanks to the system
            for (int i = 0; i < 10; i++){
                sys.addTank(i, DEFCAP, MINCAP);
                fuelAdded += MINCAP;
            }
            
            //result is tested to ensure fuelAdded equal 20000, which is MINCAP * 10
            result = result && (fuelAdded == 20000);

            //checks to ensure that totalFuel return == fuelAdded
            result = result && (sys.totalFuel() == fuelAdded);

            //a tank is removed, and it ensures that totalFuel accounts for that
            result = result && sys.removeTank(0);
            fuelAdded -= MINCAP;
            result = result && (sys.totalFuel() == fuelAdded);

            //a tank is then added, and it is checked that totalFuel accounts for its addition
            result = result && sys.addTank(10, DEFCAP, MINCAP);
            fuelAdded += MINCAP;
            result = result && (sys.totalFuel() == fuelAdded);

            //destructor for sys is called
            sys.~FuelSys();

            //result checks a final time to make sure total fuel of the system is zero
            result = result && (sys.totalFuel() == 0);

            //result is then returned
            return result;

        }

        //errorTotalFuel
        //checks to ensure that the total fuel for an error case is zero
        bool errorTotalFuel(FuelSys &sys){
            bool result = true;

            //attempts to add errored tanks
            result = result && !(sys.addTank(0, 70, -100));
            result = result && !(sys.addTank(-1, DEFCAP, DEFCAP));
            result = result && !(sys.addTank(0, DEFCAP, -1000));

            //checks to ensure that totalFuel is still equal to zero since object should still be empty
            result = result && (sys.totalFuel() == 0);

            return result;
        }

        //testAddPump
        //tests addPump and ensures that pumps are properly added
        bool testAddPump(FuelSys& sys, int numTanks){
            bool result = true;

            //randFuel and randTank declared and constructed
            Random randFuel(MINCAP,DEFCAP);

            //for loop adds pumps to the system tanks which corresponds to each tank in the list
            for (int i = 0; i < numTanks; i++){
                //for loop adds a pump for each tank within the list   
                for (int j = 0; j < numTanks; j++){
                    //ternary operator checks if j is equal to i to prevent self-pump assignment
                    int target = (i == j) ? MAX_INT : j; //if equal, MAX_INT, otherwise j
                    result = result && sys.addPump(i, j, target);
                    //result checks to ensure that m_pumps is the new pump added by testing that the pumpID and targetID are j
                    result = result && (sys.m_current->m_next->m_pumps->m_pumpID == j);
                    
                    //if statement verifies that m_next after m_pumps is not nullptr
                    if (sys.m_current->m_next->m_pumps->m_next != nullptr){
                    //result checks to make sure the ID after the one just added is the one less than it
                        result = result && (sys.m_current->m_next->m_pumps->m_next->m_pumpID == (j - 1));
                    }
                }
            }

            return result;
        }

        //errorAddPump
        //attempts to add errored pumps
        bool errorAddPump(FuelSys& sys, int numTanks){
            bool result = true;

            //adds a pump to a tank which isn't in the list yet
            result = result && !(sys.addPump((numTanks + 1), 0, 0));

            //adds a pump to a tank which was just removed
            result = result && sys.removeTank(MAX_INT);
            result = result && !(sys.addPump(MAX_INT, 8, (numTanks - 1)));

            //passes a negative, invalid tank to the list
            result = result && !(sys.addPump(-1, 0, 0));

            //passes a negative pumpID to the list
            result = result && !(sys.addPump(-1, 0, 0));

            //passes a negative target tank to the list
            result = result && !(sys.addPump(-1, 0, 0));

            //all negative add pump
            result = result && !(sys.addPump(-1, -1, -1));

            //attempts to add a pump which pumps to itself
            result = result && !(sys.addPump(0, 1, 0));

            //adds a pump to the edge tank, then adds the same pump again, then attempts to add another pump with the same ID
            result = result && sys.addTank(MAX_INT, DEFCAP, 0);
            result = result && sys.addPump(MAX_INT, 1, 0);
            result = result && !(sys.addPump(MAX_INT, 1, 0));
            result = result && !(sys.addPump(MAX_INT, 1, 54));


            return result;
        }

        //testRemovePump
        //tests removePump function
        bool testRemovePump(FuelSys& sys, int numTanks){
            bool result = true;

            //for loop remove pumps to the system tanks which corresponds to each tank in the list
            for (int i = 0; i < numTanks; i++){
                //for loop removes pump for each one within the list   
                for (int j = 0; j < numTanks; j++){
                    //result finds the tank
                    result = result && sys.findTank(i);
                    //if statement checks to make sure m_pumps does not have the ID, and it is not the one being deleted
                    if(sys.m_current->m_next->m_pumps->m_pumpID != j){
                        //tempPump and currPump are declared and initialized
                        Pump* tempPump = sys.m_current->m_next->m_pumps;
                        Pump* currPump = tempPump;

                        bool found = false;
                        while (!found){
                            //if it is found, tempPump is moved up one and found is true
                            if (tempPump->m_pumpID == j){
                                tempPump = tempPump->m_next;
                                found = true;
                            }
                            //else, both pointers are moved up
                            else{
                                //both ptrs are moved up one
                                currPump = tempPump;
                                tempPump = tempPump->m_next;
                            }
                        }
                        result = result && sys.removePump(i, j);
                    
                        //result then searches for the pump to make sure it doesn't exist within the list anymore
                        //m_current->m_next sent because findTank was called within removePump, so it should still be after m_current
                        result = result && !(findPump(*sys.m_current->m_next, j));
                        result = result && (currPump->m_next == tempPump);
                    }
                    else{
                        result = result && sys.removePump(i, j);
                        result = result && (sys.m_current->m_next->m_pumps == nullptr);
                    }
                }
            } 

            //adds a pump to the edge tank then attempts to remove it
            result = result && sys.addPump(MAX_INT, MAX_INT, 0);
            result = result && sys.removePump(MAX_INT, MAX_INT);

            //three pumps added to edge tank, tested when middle is removed and when last is removed
            result = result && sys.addPump(MAX_INT, 2, 0);
            result = result && sys.addPump(MAX_INT, 1, 0);
            result = result && sys.addPump(MAX_INT, 0, 0);
            result = result && sys.removePump(MAX_INT, 1);

            sys.findTank(MAX_INT);
            //checks to make sure the node was properly moved
            result = result && (sys.m_current->m_next->m_pumps->m_next->m_pumpID == 2);

            //node with pumpID 2 deleted and then checked
            result = result && sys.removePump(MAX_INT, 2);
            result = result && (sys.m_current->m_next->m_pumps->m_next == nullptr);

            return result;
        }

        //errorRemovePump
        //tests error cases for removing pumps
        bool errorRemovePump(FuelSys& sys, int numTanks){
            bool result = true;

            //a pump is attemped to be removed twice
            result = result && sys.addPump(MAX_INT, 0, 0);
            result = result && (sys.removePump(MAX_INT, 0));
            result = result && !(sys.removePump(MAX_INT, 0));

            //a pump which was never added is attempted to be removed from an empty tank
            result = result && !(sys.removePump(MAX_INT, 0));

            //a pump which was never added tested on a nonempty tank
            result = result && sys.addPump(MAX_INT, 7, 0);
            result = result && !(sys.removePump(MAX_INT, 10));

            //remove pump tested with negative numbers
            result = result && !(sys.removePump(0, -30));
            result = result && !(sys.removePump(-1, 0));
            result = result && !(sys.removePump(-1,-1));
            
            //tank is removed, then tested to make sure that pump cannot be found
            result = result && sys.removeTank(MAX_INT);
            result = result && !(sys.removePump(MAX_INT, MAX_INT));
            

            //result returned
            return result;
        }
        //testDrain
        //drain function is tested to ensure that it works properly
        bool testDrain(FuelSys& sys){
            bool result = true;
            //4 tanks are added, all with an m_tankFuel of 200
            for (int i = 0; i < 4; i++){
                result = result && sys.addTank(i, DEFCAP, 200);
            }

            //pumps are then added
            //for loop adds pumps to the system tanks which corresponds to each tank in the list
            for (int i = 0; i < 4; i++){
                //3-i used as target since targetTank cannot equal tankID
                result = result && sys.addPump(i, 0, (3-i));
            }

            //totalFuelAdded declared and initialized to the total amount of fuel within the tank
            int totalFuelAdded = sys.totalFuel();

            //drain is first tested upon 0 to target 3, and only 50 of it's fuel is sent over
            //pumpID corresponds to opposite, so drain will go to tankID 3
            result = result && sys.drain(0, 0, 50);
            //first checked is to make sure the totalFuel did not change, as the total amount should stay the same since it's a transfer
            result = result && (totalFuelAdded == sys.totalFuel());
            
            //second thing checked is to ensure that the targetTank was properly filled, and should have 250 fuel
            //targetTank is still m_current->m_next since it was found in fill
            result = result && (sys.m_current->m_next->m_tankFuel == (200 + 50));

            result = result && sys.findTank(0); //tank which was used as a pump
            //checks to make sure the m_tankFuel of tank 0 is 150, or 200 - 50
            result = result && (sys.m_current->m_next->m_tankFuel == (200 - 50));

            //next thing tested is tank 2 gives tank 1 250 fuel, which is more than its current amount
            //with this, tank 2 should have 0 fuel, tank 1 should be at 350 fuel, and totalFuel should remain the same
            result = result && sys.drain(2, 0, 250);

            //first checked is to make sure the totalFuel did not change, as the total amount should stay the same since it's a transfer
            result = result && (totalFuelAdded == sys.totalFuel());

            //tank 1 checked to ensure it is at 400
            result = result && (sys.m_current->m_next->m_tankFuel == (200 + 200));
            
            result = result && sys.findTank(2); //tank which was used as a pump
            //checks to make sure the m_tankFuel of tank 2 is set to 0 
            result = result && (sys.m_current->m_next->m_tankFuel == 0); 
            
            
            //with tank 2 having 0, it is tested again to ensure no fuel can transfer over
            result = result && sys.drain(2, 0, 250);
            
            //everything is rechecked to ensure nothing changed
            result = result && (totalFuelAdded == sys.totalFuel());
            result = result && (sys.m_current->m_next->m_tankFuel == (200 + 200));
            result = result && sys.findTank(2); //tank which was used as a pump
            result = result && (sys.m_current->m_next->m_tankFuel == 0); 

            //a new tank is added, and fills tank 1 past capacity
            result = result && sys.addTank(4, DEFCAP, DEFCAP);
            totalFuelAdded += DEFCAP; 
            result = result && sys.addPump(4, 0, 1);
            result = result && sys.drain(4, 0, DEFCAP);

            //tank 1 tested to ensure it is at DEFCAP
            result = result && (sys.m_current->m_next->m_tankFuel == DEFCAP);

            sys.findTank(4);
            //since tank 1 had 400, tank 4 should have 400 leftover since not filled to capacity
            result = result && (sys.m_current->m_next->m_tankFuel == 400);
            result = result && (totalFuelAdded == sys.totalFuel());

            //result is then returned
            return result;
        }

        //randomDrain
        //tests drain with random cases of setTest
        bool randomDrain(FuelSys& sys, int numTanks){
            bool result = true;

            //totalFuelAdded is the amount of fuel within the system currently
            int totalFuelAdded = sys.totalFuel();
            

            //each pump is known to have 3 pumps, so each tank is drained with a random number
            for (int i = 0; i < numTanks; i++){
                for (int j = 0; j < numTanks; j++){
                    Random randFuelDrain(0, 10000); //randFuelDrain is a random amount of fuel which may be transferred
                    result = result && sys.drain(i, j, randFuelDrain.getRandNum());
                }
            }
            //regardless of the pump differences, totalFuel should always remain equal to totalFuelAdded, so this is finally checked
            result = result && (totalFuelAdded = sys.totalFuel());
            return result;
        }

        //errorDrain
        //drain tested for error cases
        bool errorDrain(FuelSys& sys, int numTanks){
            bool result = true;

            //totalFuelAdded declared and initialized
            int totalFuelAdded = sys.totalFuel();

            //tank 0 is removed from the list, and then drain is tested to ensure it doesn't work
            result = result && sys.findTank(0);
            result = result && sys.addPump(MAX_INT, 0, 0); //pump added to edge tank
            totalFuelAdded -= (sys.m_current->m_next->m_tankFuel); //m_tankFuel of 0 subtracted from totalFuelAdded due to removal
            result = result && sys.removeTank(0);
            result = result && !(sys.drain(MAX_INT, 0, 1000));

            //drain with negative numbers are tested to ensure that it doesn't work
            result = result && !(sys.drain(-3, 1, 1000)); //negative tank ID
            result = result && !(sys.drain(1, -5, 1000)); //negative pumpID
            result = result && !(sys.drain(1, 5, -1000)); //negative fuel
            result = result && !(sys.drain(-1, -1, -4000)); //all negative
            
            //if statement checks that a normal case is being tested
            if (numTanks == NORMAL_CASE){
                //a pump is removed, and it is tested to ensure that it does not still work
                result = result && sys.removePump(1, 2);
                result = result && !(sys.drain(1, 2, 100));
            }

            //0 tank re-added with a fuel of 0, attempted drain with 0 fuel
            result = result && sys.addTank(0, DEFCAP, 0);
            result = result && sys.addPump(0, 0, MAX_INT);
            result = result && !(sys.drain(0, 0, 0));

            //result tested to ensure that totalFuelAdded equals sys.totalFuel()
            result = result && (totalFuelAdded = sys.totalFuel());
            return result;
        }

        //testAssignment
        //tests the assignment operator to ensure that it works properly
        bool testAssignment(FuelSys& pureEdge, FuelSys& IDEdge, FuelSys& normalSys){
            bool result = true;

            FuelSys* tempSys = new FuelSys(); //tempSys declared and initialized, deleted later
            *tempSys = normalSys; //tempSys is a placeholder for normal
            
            Tank* tempSysCurr = tempSys->m_current;
            Tank* normalSysCurr = normalSys.m_current;

            //do while loop goes through both objects to ensure that they both are equal
            do{
                //all member variables of tank are checked
                result = result && (tempSysCurr->m_tankID == normalSysCurr->m_tankID);
                result = result && (tempSysCurr->m_tankCapacity == normalSysCurr->m_tankCapacity);
                result = result && (tempSysCurr->m_tankFuel == normalSysCurr->m_tankFuel);
                
                //Pump pointers normalPumpCurr and tempPumpCurr declared and initialized to m_pumps of both respective pointers
                Pump* normalPumpCurr = normalSysCurr->m_pumps;
                Pump* tempPumpCurr = tempSysCurr->m_pumps;

                //while loop goes through pump and checks to ensure that all pump member variables are the same
                while (normalPumpCurr != nullptr){
                    //result is dependent on whether both member variables from Pump class for each one in the singly linked list are equal
                    result = result && (normalPumpCurr->m_pumpID == tempPumpCurr->m_pumpID);
                    result = result && (normalPumpCurr->m_target == tempPumpCurr->m_target);

                    //both pump pointers move up one
                    normalPumpCurr = normalPumpCurr->m_next;
                    tempPumpCurr = tempPumpCurr->m_next;
                }

                //both tempSysCurr and normalSysCurr move up one
                tempSysCurr = tempSysCurr->m_next;
                normalSysCurr = normalSysCurr->m_next;

            } while (normalSysCurr != normalSys.m_current);

            
            normalSys = pureEdge; //normal first equals the edge case

            //result checks that m_current is the same for both with only one, which is verified by m_current->m_next has no pumps
            result = result && (normalSys.m_current->m_next->m_pumps == nullptr);
            result = result && (normalSys.m_current->m_tankID == MAX_INT); //checks to make sure tankIDs equal the same

            normalSys = IDEdge; //normalSys is equal to IDEdge

            //IDEdgeCurr declared and initialized, normalSysCurr set to new m_current
            Tank* IDEdgeCurr = IDEdge.m_current;
            normalSysCurr = normalSys.m_current;

            //do while loop goes through both objects to ensure that they both are equal
            do{
                //all member variables of tank are checked
                result = result && (IDEdgeCurr->m_tankID == normalSysCurr->m_tankID);
                result = result && (IDEdgeCurr->m_tankCapacity == normalSysCurr->m_tankCapacity);
                result = result && (IDEdgeCurr->m_tankFuel == normalSysCurr->m_tankFuel);
               
                //Pump pointers IDEdge and normalTwoPumpCurr declared and initialized to m_pumps of both respective pointers
                Pump* normalPumpCurr = normalSysCurr->m_pumps;
                Pump* edgePumpCurr = IDEdgeCurr->m_pumps;


                //while loop goes through pump and checks to ensure that all pump member variables are the same
                while (edgePumpCurr != nullptr){
                    //result is dependent on whether both member variables from Pump class for each one in the singly linked list are equal
                    result = result && (edgePumpCurr->m_pumpID == normalPumpCurr->m_pumpID);
                    result = result && (edgePumpCurr->m_target == normalPumpCurr->m_target);


                    //both pump pointers move up one
                    edgePumpCurr = edgePumpCurr->m_next;
                    normalPumpCurr = normalPumpCurr->m_next;
                }

                //both IDEdgeCurr and normalSysCurr move up one
                IDEdgeCurr = IDEdgeCurr->m_next;
                normalSysCurr = normalSysCurr->m_next;


            } while (IDEdgeCurr != IDEdge.m_current);
            
            delete tempSys;
            
            return result;
        }

        //testSelfAssignment
        //This function tests that self assignment does not occur
        bool testSelfAssignment(FuelSys& sys){
            bool result = true;
            
            FuelSys* tempSys = new FuelSys(); //tempSys declared and initialized, deleted later
            *tempSys = sys; //tempSys is a placeholder for normal

            //sys set equal to itself
            sys = sys;

            //result first checks that sys's m_current does not equal nullptr
            result = result && (sys.m_current != nullptr);

            //temp and curr pointers declared and initialized, and go throigh the list
            Tank* temp = tempSys->m_current;
            Tank* curr = sys.m_current;

            //for loop checks to make sure everything is equal to tempSys and that no changes were made
            do{
                //all member variables of tank are checked
                result = result && (temp->m_tankID == curr->m_tankID);
                result = result && (temp->m_tankCapacity == curr->m_tankCapacity);
                result = result && (temp->m_tankFuel == curr->m_tankFuel);
               
                //Pump pointers tempPump and currPump declared and initialized to m_pumps of both respective pointers
                Pump* tempPump = temp->m_pumps;
                Pump* currPump = curr->m_pumps;


                //while loop goes through pump and checks to ensure that all pump member variables are the same
                while (tempPump != nullptr){
                    //result is dependent on whether both member variables from Pump class for each one in the singly linked list are equal
                    result = result && (tempPump->m_pumpID == currPump->m_pumpID);
                    result = result && (tempPump->m_target == currPump->m_target);


                    //both pump pointers move up one
                    tempPump = tempPump->m_next;
                    currPump = currPump->m_next;
                }


                //both temp and curr move up one
                temp = temp->m_next;
                curr = curr->m_next;


            } while (temp != tempSys->m_current);

            delete tempSys; //tempSys deleted
            return result;
        }

        //testFill
        //tests fill by adding fuel to the tank
        bool testFill(FuelSys& sys, int numTanks){
            bool result = true;
            Random randFuel(0, DEFCAP); //randFuel
            //for loop goes through and adds a random number, and tests to ensure that each one works properly
            for (int i = 0; i < numTanks; i++){
                int randNum = randFuel.getRandNum(); //random number gotten
                int totalFuel = sys.totalFuel(); //totalFuel is sys.TotalFuel() prior to fill

                sys.findTank(i); //i tank found

                //startingFuel is starting value of m_current->m_next->m_tankFuel
                int startingFuel = sys.m_current->m_next->m_tankFuel;
                result = result && sys.fill(i, randNum); //i is filled with randNum fuel

                //fuelDifference and newFuel calculate the amount of fuel added to the system using ternary operator
                int fuelDifference = (sys.m_current->m_next->m_tankFuel - startingFuel);
                int newFuel = ((randNum <= fuelDifference) ? randNum : fuelDifference);

                result = result && (sys.totalFuel() == (totalFuel + newFuel)); //totalFuel is then compared
            }

            //final case is for MAX_INT, which is the edge tank
            int randNum = randFuel.getRandNum(); //random number gotten
            int totalFuel = sys.totalFuel(); //totalFuel is sys.TotalFuel() prior to fill

            sys.findTank(MAX_INT); //MAX_INT tank found

            //startingFuel is starting value of m_current->m_next->m_tankFuel
            int startingFuel = sys.m_current->m_next->m_tankFuel;
            result = result && sys.fill(MAX_INT, randNum); //i is filled with randNum fuel

            //fuelDifference and newFuel calculate the amount of fuel added to the system using ternary operator
            int fuelDifference = (sys.m_current->m_next->m_tankFuel - startingFuel);
            int newFuel = ((randNum <= fuelDifference) ? randNum : fuelDifference);
            result = result && (sys.totalFuel() == (totalFuel + newFuel)); //totalFuel is then compared

            //MAX_INT tank removed, readded with a capacity of 2000, then filled past capacity
            result = result && sys.removeTank(MAX_INT);
            result = result && sys.addTank(MAX_INT, MINCAP, 0); //tank readded back
            result = result && sys.fill(MAX_INT, 2001); //filled past capacity
            result = result && (sys.m_current->m_next->m_tankFuel == MINCAP);

            return result;
        }
        //errorFill
        //tests error cases for fill
        bool errorFill(FuelSys& sys, int numTanks){
            bool result = true;

            //remove edgeTank MAX_INT, then try to fill it to ensure it doesn't work
            result = result && sys.removeTank(MAX_INT);
            result = result && !(sys.fill(MAX_INT, 2000));

            //drain with negative numbers are tested to ensure that it doesn't work
            result = result && !(sys.fill(-3, 30)); //negative tank ID
            result = result && !(sys.fill(1, -50)); //negative negative fuel
            result = result && !(sys.fill(-7,-90)); //all negative

            //0 tank re-added with a fuel of 0, attempted to fill with zero fuel
            result = result && sys.addTank(MAX_INT, DEFCAP, 0);
            result = result && !(sys.fill(MAX_INT,0)); //attempts to fill with zero fuel

            return result;
        }
};

int main(){
    Tester tester;
    
    cout << "**********************" << endl;
    cout << "***** BEGIN TEST *****" << endl;
    cout << "**********************" << endl << endl;
    {
        cout << "\n*** TEST BLOCK ONE ***" << endl << endl;
        cout << "First test block will call upon testAddTank" << endl;
        cout << "Different unique tanks will be added" << endl << endl;
        
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();

        //addTank test is commenced, and different tanks are added within this function
        bool testResult = tester.testAddTank(*newSys);
        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK ONE***" << endl;
    }

    {
        cout << "\n*** TEST BLOCK TWO ***" << endl << endl;
        cout << "Second test block will call upon errorAddTank" << endl;
        cout << "Errored tanks will be added and tested to ensure they aren't added" << endl << endl;
        
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        //one tank added to newSys for testing
        newSys->addTank(0, DEFCAP, 0);

        //errorAddTank is tested to see if it works
        bool testResult = tester.errorAddTank(*newSys);
        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK TWO***" << endl;
    }

    {
        cout << "\n*** TEST BLOCK THREE ***" << endl << endl;
        cout << "Third test block will call upon testRemoveTank" << endl;
        cout << "setTest called three different times, and tanks will be removed in normal order, refilled, then reverse order" << endl << endl;
        
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        cout << "\nPure Edge Case: \n\t";

        //first tested is the edge case
        //if it works correctly, then testcondition will prove true
        tester.setTest(*newSys, PURE_EDGE_CASE);

        //testRemoveTank tested
        testResult = tester.testRemoveTank(*newSys, PURE_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys; //newSys deleted
        
        //ID Edge Case tested
        cout << "\nID Edge Case: \n\t";
        newSys = new FuelSys();

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        //testRemoveTank tested
        testResult = tester.testRemoveTank(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys; //newSys deleted for normal case


        //third test is with normal case
        cout << "\nNormal Case: \n\t";

        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);

        testResult = tester.testRemoveTank(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK THREE ***" << endl;
    }
    {
        cout << "\n*** TEST BLOCK FOUR ***" << endl << endl;
        cout << "Fourth test block will call upon errorRemoveTank" << endl;
        cout << "setTest will be called for both edge cases and the normal case" << endl;
        cout << "removeTank will be called improperly and some tanks will be attemped to be removed twice" << endl << endl;
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;
        cout << "\nPure Edge Case: \n\t";

        //first tested is the pure edge case
        //if it works correctly, then testcondition will prove true
        tester.setTest(*newSys, PURE_EDGE_CASE);

        testResult = tester.errorRemoveTank(*newSys, PURE_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys; //newSys deleted for ID Edge Case

        cout << "\nID Edge Case: \n\t";

        newSys = new FuelSys(); //newSys re-initialized

        //next tested is the ID Edge Case, with two tanks in the list, both with the lowest and highest possible ID
        tester.setTest(*newSys, ID_EDGE_CASE);

        testResult = tester.errorRemoveTank(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;
        
        newSys = new FuelSys();

        //final test is with normal case
        cout << "\nNormal Case: \n\t";
        
        //setTest done for a normal case
        tester.setTest(*newSys, NORMAL_CASE);

        //errorRemoveTank tested for a normal case
        testResult = tester.errorRemoveTank(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK FOUR ***" << endl;        
    }
    {
        cout << "\n*** TEST BLOCK FIVE ***" << endl << endl;
        cout << "Fifth test block will call upon testFindTank" << endl;
        cout << "setTest will be called three times for two edge cases and a normal case" << endl;
        cout << "Each tank within the system will be found and checked to ensure it is m_current->m_next" << endl << endl;
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        cout << "\nPure Edge Case: \n\t";

        //first tested is the pure edge case
        tester.setTest(*newSys, PURE_EDGE_CASE);

        //findTank tested with the pure edge case
        testResult = tester.testFindTank(*newSys, PURE_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //ID Edge Case tested, with the highest and lowest possible IDs
        cout << "\nID Edge Case: \n\t";
        newSys = new FuelSys();

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        //findTank is tested
        testResult = tester.testFindTank(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //final test is with normal case
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);

        //findTank is tested with the normal tank
        testResult = tester.testFindTank(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK FIVE ***" << endl;  
    } 
    {
        cout << "\n*** TEST BLOCK SIX ***" << endl << endl;
        cout << "Sixth test block will call upon errorFindTank" << endl;
        cout << "setTest will be called three times, and findTank will be tested for errors" << endl << endl;
        
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        cout << "\nPure Edge Case: \n\t";

        //first tested is the pure edge case
        tester.setTest(*newSys, PURE_EDGE_CASE);

        //errorFindTank tested with the pure edge case
        testResult = tester.errorFindTank(*newSys, PURE_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\nID Edge Case: \n\t";
        
        newSys = new FuelSys();
        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        testResult = tester.errorFindTank(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //final test is with normal case
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);

        //errorFindTank tested for normal case
        testResult = tester.errorFindTank(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK SIX ***" << endl;  
    } 
    {
        cout << "\n*** TEST BLOCK SEVEN ***" << endl << endl;
        cout << "Seventh test block will call upon testTotalFuel" << endl;
        cout << "Due to the nature of testTotalFuel, it is only called once to keep proper track of the number of fuel" << endl;
        cout << "testTotalFuel will add a certain amount of fuel, and test it to ensure it is the proper value" << endl << endl;
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        //testTotalFuel called and then the result of the test is outputted by testConditon
        bool testResult = tester.testTotalFuel(*newSys);
        tester.testCondition(testResult);


        delete newSys;
        cout << "\n***END TEST BLOCK SEVEN ***" << endl;  
    }
    {
        cout << "\n*** TEST BLOCK EIGHT ***" << endl << endl;
        cout << "Eighth test block will call upon errorTotalFuel" << endl;
        cout << "Due to the nature of errorTotalFuel, it is only called once to keep proper track of the number of fuel" << endl;
        cout << "Errored fuels will be added, and it will be ensured that totalFuel does not change" << endl;
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        //errorTotalFuel called and then the result of the test is outputted by testConditon
        bool testResult = tester.errorTotalFuel(*newSys);
        tester.testCondition(testResult);


        delete newSys;
        cout << "\n***END TEST BLOCK EIGHT ***" << endl;  
    }
    {
        cout << "\n*** TEST BLOCK NINE ***" << endl << endl;
        cout << "Ninth test block will call upon testAddPump" << endl;
        cout << "emptyTankAddition will be called three times for the ID edge case and a normal case" << endl;
        cout << "This function adds pumps and ensures that they are being added properly" << endl << endl;
        
        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        //pure edge case cannot be tested since a pump cannot hold onto itself
        //ID Edge Case is still possible to test, however
        cout << "\nID Edge Case: \n\t";

        //setTest done for the ID Edge Case
        tester.emptyTankAddition(*newSys, ID_EDGE_CASE);
        
        testResult = tester.testAddPump(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //second test is with normal case
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.emptyTankAddition(*newSys, NORMAL_CASE);

        //testAddPump tested for a normal case
        testResult = tester.testAddPump(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK NINE ***" << endl;
    }
    {
        cout << "\n*** TEST BLOCK TEN ***" << endl << endl;
        cout << "Tenth test block will call upon errorAddPump" << endl;
        cout << "setTest will be called for all three cases, and each will test errorAddPump" << endl;
        cout << "Errored pumps will attempt to be added, but to no avail" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        //pure edge case cannot be tested since a pump cannot hold onto its own ID
        //ID edge case tested
        cout << "\nID Edge Case: \n\t";

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        //errorAddPump tested for the ID edge case
        testResult = tester.errorAddPump(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //final test is with normal case
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);
        
        //errorAddPump finally tested for the normal case
        testResult = tester.errorAddPump(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK TEN ***" << endl; 
    }
    {
        cout << "\n*** TEST BLOCK ELEVEN ***" << endl << endl;
        cout << "Eleventh test block will call upon testRemovePump" << endl;
        cout << "setTest will be called two times, and removePump will be tested" << endl;
        cout << "Pumps will be removed, and checked to ensure they removed properly" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        bool testResult = true;

        //ID Edge Case tested since Pure Edge cannot be tested due to pump target not being able to equal tankID
        cout << "\nID Edge Case: \n\t";

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);

        testResult = tester.testRemovePump(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //final test is with normal case
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);

        testResult = tester.testRemovePump(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);
        
        delete newSys;
        cout << "\n***END TEST BLOCK ELEVEN ***" << endl;  
    } 
    {
        cout << "\n*** TEST BLOCK TWELVE ***" << endl << endl;
        cout << "Twelfth test block will call upon errorRemovePump" << endl;
        cout << "setTest will be called two times for the ID edge case and the normal case" << endl;
        cout << "Errored pumps will attempt to be removed, and it will check to ensure that nothing changes" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;
        
        //Only ID Edge Case tested since Pure Edge cannot be tested due to pump target not being able to equal tankID
        cout << "\nID Edge Case: \n\t";

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        //errorRemovePump called and result is tested
        testResult = tester.errorRemovePump(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;
        
        //final test is with normal case
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);

        //errorRemovePump called for normal case
        testResult = tester.errorRemovePump(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;

        cout << "\n***END TEST BLOCK TWELVE ***" << endl;
    }
    {
        cout << "\n*** TEST BLOCK THIRTEEN ***" << endl << endl;
        cout << "Thirteenth test block will call upon testDrain" << endl;
        cout << "Due to the nature of testDrain, setTest is not called since the fuel inserted needs to be kept track of" << endl;
        cout << "Pumps are added, drained, and ensured that they work properly and that value remain the same" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        //testDrain called and then the result of the test is outputted by testConditon
        bool testResult = tester.testDrain(*newSys);
        tester.testCondition(testResult);


        delete newSys;
        cout << "\n***END TEST BLOCK THIRTEEN ***" << endl;  
    }
    {
        cout << "\n*** TEST BLOCK FOURTEEN ***" << endl << endl;
        cout << "Fourteenth test block will call upon randomDrain" << endl;
        cout << "setTest will be called two times for an edge case and a normal case" << endl;
        cout << "Drain will be called upon random pumps, and it will be checked to ensure that all values are correct" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        //only ID Edge cased called since the pure edge case is not random at all
        cout << "\nID Edge Case: \n\t";

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        testResult = tester.randomDrain(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //final test is with normal case
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);
        
        //randomDrain tested
        testResult = tester.randomDrain(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK FOURTEEN ***" << endl;
    }
    {
        cout << "\n*** TEST BLOCK FIFTEEN ***" << endl << endl;
        cout << "Fifteenth test block will call upon errorDrain" << endl;
        cout << "setTest will be called two times for an edge case and a normal case" << endl;
        cout << "Drain will be called for error cases to ensure that it does not work" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;
        
        //only ID edge case done because errorDrain cannot be properly tested with the pure edge case
        cout << "\nID Edge Case: \n\t";

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        //error drain called for id edge case
        testResult = tester.errorDrain(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //final test is with normal case
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);

        //errorDrain called for the normal case
        testResult = tester.errorDrain(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK FIFTEEN ***" << endl;
    }
    {
        cout << "\n*** TEST BLOCK SIXTEEN ***" << endl << endl;
        cout << "Sixteenth test block will call upon testAssignment" << endl;
        cout << "setTest will be called three times for two edge cases and a normal case" << endl;
        cout << "All three will be sent to testAssignment, and then compared within that function" << endl << endl;

        //three different Fuelsys objects are declared and created
        FuelSys* edgeSys = new FuelSys();
        FuelSys* IDEdgeSys = new FuelSys();
        FuelSys* normalSys = new FuelSys();

        //all three pointer objects call setTest
        tester.setTest(*edgeSys, PURE_EDGE_CASE);
        tester.setTest(*IDEdgeSys, ID_EDGE_CASE);
        tester.setTest(*normalSys, NORMAL_CASE);
    
        //all three objects sent to the assignment operator test
        bool testResult = tester.testAssignment(*edgeSys, *IDEdgeSys, *normalSys);
        tester.testCondition(testResult);
        
        delete edgeSys;
        delete IDEdgeSys;
        delete normalSys;

        cout << "\n***END TEST BLOCK SIXTEEN ***" << endl;
    } 
    {
        cout << "\n*** TEST BLOCK SEVENTEEN ***" << endl << endl;
        cout << "Seventeenth test block will call upon testSelfAssignment" << endl;
        cout << "setTest will be called three times for two edge cases and a normal case" << endl;
        cout << "This checks to ensure that no self assignment occurs where this becomes empty" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        //Pure Edge Case is tested to see if no self assignment occurs
        cout << "\nPure Edge Case: \n\t";

        tester.setTest(*newSys, PURE_EDGE_CASE);

        //testSelfAssigment tested for the pure edge case
        testResult = tester.testSelfAssignment(*newSys);

        tester.testCondition(testResult);

        delete newSys;

        //second test done is on the ID Edge Case to ensure no self assignment occurs
        cout << "\nID Edge Case: \n\t";
        newSys = new FuelSys();

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        //testSelfAssignment tested for the ID Edge Case
        testResult = tester.testSelfAssignment(*newSys);

        tester.testCondition(testResult);

        delete newSys;

        //final test is done with the normal case to ensure no self assignment happens
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);
    
        //testSelfAssignment called upon the normal case
        testResult = tester.testSelfAssignment(*newSys);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK SEVENTEEN ***" << endl;
    }
    {
        cout << "\n*** TEST BLOCK EIGHTEEN ***" << endl << endl;
        cout << "Seventeenth test block will call upon testFill" << endl;
        cout << "setTest will be called three times for two edge cases and a normal case" << endl;
        cout << "fill is tested to ensure that it properly fills tanks and that totalFuel properly changes" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        //Pure Edge Case is tested to see if testFill works properly
        cout << "\nPure Edge Case: \n\t";

        tester.setTest(*newSys, PURE_EDGE_CASE);

        //testFill tested for the pure edge case
        testResult = tester.testFill(*newSys, PURE_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //second test done is on the ID Edge Case to ensure testFill works properly
        cout << "\nID Edge Case: \n\t";
        newSys = new FuelSys();

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        //testFill tested for the ID Edge Case
        testResult = tester.testFill(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //final test is done with the normal case to ensure testFill works properly
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);
    
        //testFill called upon the normal case
        testResult = tester.testFill(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK EIGHTEEN ***" << endl;
    }
    {
        cout << "\n*** TEST BLOCK NINETEEN ***" << endl << endl;
        cout << "Seventeenth test block will call upon errorFill" << endl;
        cout << "setTest will be called three times for two edge cases and a normal case" << endl;
        cout << "fill will be called for error cases to ensure that it does not change values" << endl << endl;

        //newSys declared and initialized
        FuelSys* newSys = new FuelSys();
        
        bool testResult = true;

        //Pure Edge Case is tested to see if errorFill works properly
        cout << "\nPure Edge Case: \n\t";

        tester.setTest(*newSys, PURE_EDGE_CASE);

        //errorFill tested for the pure edge case
        testResult = tester.errorFill(*newSys, PURE_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //second test done is on the ID Edge Case to ensure errorFill works properly
        cout << "\nID Edge Case: \n\t";
        newSys = new FuelSys();

        //setTest done for the ID Edge Case
        tester.setTest(*newSys, ID_EDGE_CASE);
        
        //errorFill tested for the ID Edge Case
        testResult = tester.errorFill(*newSys, ID_EDGE_CASE);

        tester.testCondition(testResult);

        delete newSys;

        //final test is done with the normal case to ensure errorFill works properly
        cout << "\nNormal Case: \n\t";
        newSys = new FuelSys();

        tester.setTest(*newSys, NORMAL_CASE);
    
        //errorFill called upon the normal case
        testResult = tester.errorFill(*newSys, NORMAL_CASE);

        tester.testCondition(testResult);

        delete newSys;
        cout << "\n***END TEST BLOCK NINETEEN ***" << endl;
    }

    cout << "\n**********************" << endl;
    cout << "***** END TEST *******" << endl;
    cout << "**********************" << endl;
    
    return 0;
}