// UMBC - CMSC 341 - Spring 2023 - Proj1
/***************************
 ** File:    fuel.cpp
 ** Project: CMSC 341, proj0, Spring 2023
 ** Author:  William Whatley
 ** Date:    2/15/2023
 ** Section: Marron
 ** Email:   w59@umbc.edu
 **
 ** This file defines the functions laid out in fuel.h. Every function is initialized here
 **
 ** All functions were created by William Whatley excluding dumpSys, whch was provided in the project
*****************************/
#include "fuel.h"

FuelSys::FuelSys(){
    m_current = nullptr;
}
FuelSys::~FuelSys(){
    //if statement checks to ensure that m_current does not equal nullptr
    if (m_current != nullptr){
        Tank* prev = m_current;
        Tank* curr = m_current;

        //do while loop goes through the entire linked list and deletes everything except m_current
        do{
            curr = curr->m_next; //curr moves up one
            
            //if statement checks if prev's m_pumps is not a nullptr, and if it isn't it deletes all pumps
            if (prev->m_pumps != nullptr){
                //pumpPrev and pumpCurr declared and initalized to m_pumps and used to traverse the linked list
                Pump* pumpPrev = prev->m_pumps;
                Pump* pumpCurr = prev->m_pumps;
                //do while loop goes through and deletes everything in m_pumps
                while (pumpCurr != nullptr){
                    pumpCurr = pumpCurr->m_next; //pumpCurr moves up one
                    delete pumpPrev; //pumpPrev deleted
                    pumpPrev = pumpCurr; //pumpPrev = pumpCurr
                }
            }

            delete prev; //prev is deleted

            prev = curr; //prev equals curr so it goes to a node

        } while (curr != m_current); //while curr != m_current, the starting place

        m_current = nullptr; //m_current set back to nullptr
    }
}

//addTank
//attempts to add a new tank/node to the linked list, returns true or false whether or not the action was performed
bool FuelSys::addTank(int tankID, int tankCap, int tankFuel = 0) {
    //if statement verifies that the tank object is valid to be created and added to the linked list
    //first if statement verifies that tankCap is a valid size
    if ((tankCap < MINCAP)){
        return false;
    }
    //if statement checks that tankID and tankFuel are not negative numbers
    else if ((tankID < 0) || (tankFuel < 0)){;
        return false;
    }
    //else checks to see if the list is empty
    else if (m_current == nullptr){
        //newTank object dynamically created
        Tank* newTank = new Tank(tankID, tankCap, tankFuel);
        m_current = newTank; //m_current points to newTank
        m_current->m_next = newTank; //m_current points to itself since it's a circular linked list
        return true;
    }
    //if tankID equals the tank ID of all of the tanks, then that object already exists in this list and cannot be added
    else if (findTank(tankID)){
        return false;
    }
    else{
        //if the object above doesn't fall into the 'pitfalls', then it can be created and added to the list
        Tank* newTank = new Tank(tankID, tankCap, tankFuel);

        //the object is added to m_current-> m_next, and its m_next is set to the m_next, and it's set to m_current
        newTank->m_next = m_current->m_next;
        m_current->m_next = newTank;
        m_current = newTank;

        //once it's all done, it returns true
        return true;
    }
}
bool FuelSys::removeTank(int tankID){
    //if statement checks to make sure nullptr and tankID is not negative
    if (m_current == nullptr){
        return false;
    }
    else if (tankID < 0){
        return false;
    }
    else if (findTank(tankID)){
        //Tank pointer curr declared and initialized
        Tank* curr = m_current->m_next;
        Tank* prev = m_current;

        //if statement checks to see if prev = curr, which means that m_current is pointing to itself so only node in the list
        if (prev == curr){
            //deletes all pump within m_current
            if (curr->m_pumps != nullptr){
                //pumpPrev and pumpCurr declared and initalized to m_pumps and used to traverse the linked list
                Pump* pumpPrev = curr->m_pumps;
                Pump* pumpCurr = curr->m_pumps;
                //do while loop goes through and deletes everything in m_pumps
                while (pumpCurr != nullptr){
                    pumpCurr = pumpCurr->m_next; //pumpCurr moves up one
                    delete pumpPrev; //pumpPrev deleted
                    pumpPrev = pumpCurr; //pumpPrev = pumpCurr
                }
            }
            delete prev; //prev deleted
            m_current = nullptr; //m_current set to nullptr
        }
        else{
            //prev's m_next is set to curr m_next so curr is out of the list
            prev->m_next = curr->m_next;

            //if statement checks if curr's m_pumps is not a nullptr, and if it isn't it deletes all pumps
            if (curr->m_pumps != nullptr){
                //pumpPrev and pumpCurr declared and initalized to m_pumps and used to traverse the linked list
                Pump* pumpPrev = curr->m_pumps;
                Pump* pumpCurr = curr->m_pumps;
                //do while loop goes through and deletes everything in m_pumps
                while (pumpCurr != nullptr){
                    pumpCurr = pumpCurr->m_next; //pumpCurr moves up one
                    delete pumpPrev; //pumpPrev deleted
                    pumpPrev = pumpCurr; //pumpPrev = pumpCurr
                }
            }
            //curr is deleted
            delete curr;

            curr = nullptr;
        }
        //true is returned if it is completed
        return true;
    }
    //if none are true, it means the tank doesnt exist and false is returned
    else{
        return false;
    }
}

//findTank
//Checks to see if a tank with that ID exists in the loop, and if so sets that to m_current
bool FuelSys::findTank(int tankID){
    //do while loop goes through the entire linked list to see 
    if (m_current == nullptr){
        return false;
    }
    //else statement ensures that a negative ID isn't sent
    else if (tankID < 0){
        return false;
    }
    else{
        //curr declared and initialized
        Tank* curr = m_current;

        //do while goes through and moves m_current if the tank exists, if not it returns false
        do{
            //if statement checks if the curr->m_next has the tankID, and if it does m_current becomes curr
            if (curr->m_next->m_tankID == tankID){
                m_current = curr;
                return true;
            }
            curr = curr->m_next;
        }while (curr != m_current);
        return false; //if it goes through the circular linked list and can't find anything, returns false
    }
}

//addPump
//Adds a pump to the singly linked list within the tank
bool FuelSys::addPump(int tankID, int pumpID, int targetTank){
    //if statement checks to make sure all integers are positive and tankID does not equal targetTank
    if ((tankID >= 0) && (pumpID >= 0) && (targetTank >= 0) && (tankID != targetTank)){
        //function checks to ensure that the tank exists by using findTank
        if (findTank(targetTank)){
            //function checks to ensure that the tank exists by using findTank
            if (findTank(tankID)){
                //curr declared and initialized to m_current
                //curr is set to m_current->m_next->m_next, as findTank moved the tankID to two behind due to findTank (targetTank)
                Tank* curr = m_current->m_next;
        
                //if statement checks if m_pumps is empty, indicated by curr->m_pumps == nullptr
                if(curr->m_pumps == nullptr){
                    //newPump object created and added to the pump list
                    Pump* newPump = new Pump(pumpID, targetTank);
                    curr->m_pumps = newPump; //m_pumps is the head, since there's only one it's the node
                    return true;
                }
                else{
                    Pump* pumpCurr = curr->m_pumps; //pumpCurr declared and initialzed to the head of m_pumps
                    
                    //while loop goes through the pump list to ensure that the pump ID is not being used already
                    while (pumpCurr != nullptr){
                        //if the pumpID already exists, it returns false
                        if (pumpCurr->m_pumpID == pumpID){
        
                            return false;
                        }
                        pumpCurr = pumpCurr->m_next;
                    }
                    //newPump object created, the m_next for pumps is the m_next for pumps of the current pointer
                    pumpCurr = new Pump(pumpID, targetTank, curr->m_pumps);
                    curr->m_pumps = pumpCurr; //m_pumps is the head, since there's only one it's the node
                    return true;
                }
            }
            else{
                return false;
            }
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

//removePump
//removes pump from the linked list of the list ID
bool FuelSys::removePump(int tankID, int pumpID){
    //if statement checks to ensure negative numbers aren't being passed
    if ((tankID < 0) || (pumpID < 0)){
        return false;
    }
    //function checks to ensure that the tank exists by using findTank
    else if (findTank(tankID)){
        Tank* curr = m_current->m_next; //curr declared and initialized as findTank moved it to the next

        //if statement checks that curr->m_pumps does not equal nullptr
        if (curr->m_pumps == nullptr){
            return false;
        }
        //if curr->m_pumps->m_pumpID is false, but m_next is nullptr, it means it is not in the list
        else if ((curr->m_pumps->m_pumpID != pumpID) && (curr->m_pumps->m_next == nullptr)){
            return false;
        }
        //if statement checks if the pump being removed is the first one
        else if (curr->m_pumps->m_pumpID == pumpID){
            Pump* pumpCurr = curr->m_pumps; //pumpCurr declared and initalized
            curr->m_pumps = curr->m_pumps->m_next; //m_pumps next is set to the next in the list
            delete pumpCurr; //pumpCurr is deleted
            return true;
        }
        else{
            Pump* pumpCurr = curr->m_pumps; //pumpCurr declared and initialized to m_pumps
            Pump* pumpPrev = curr->m_pumps;
            //do while goes through the singly linked list to ensure that a pump with that ID exists
            while (pumpCurr != nullptr){
                pumpCurr = pumpCurr->m_next;
                //if statement checks if the pumpID equals m_pumpID
                if (pumpCurr->m_pumpID == pumpID){
                    //the m_next of the previous node equals the m_next of the current node
                    pumpPrev->m_next = pumpCurr->m_next;
                    delete pumpCurr; //pumpCurr deleted
                    return true;
                }
                
                //pumpPrev stays one behind pumpcurr by equaling pumpCurr while pumpCurr moves up one
                pumpPrev = pumpCurr;
            }
            
            //if end of do while loop is reached with no pumpID, then it returns false;
            return false; 
        }
    }
    else{
        return false;
    }
}

//totalFuel
//adds up all of the fuel in the system and returns it
int FuelSys::totalFuel() const{
    //if statement checks to ensure that the list is not empty
    if (m_current == nullptr){
        return 0;
    }
    else{
        //totalFuel declared and initalized to zero
        int totalFuel = 0;
        //curr declared and initialized
        Tank* curr = m_current;

        //do while loop goes through the linked list and adds all the fuel to totalFuel
        do{
            //totalFuel is equal to totalFuel plus m_tankFuel of the current tank
            totalFuel += curr->m_tankFuel;
            //curr moved up one
            curr = curr->m_next;
        } while (curr != m_current);
        return totalFuel; //totalFuel returned
    }
}

//drain
//uses a pump to transfer fuel from tankID to the target tank
bool FuelSys::drain(int tankID, int pumpID, int fuel){
    //if statement goes through the linked list and sees if the tankID exists
    if (findTank(tankID) && (fuel > 0)){
        Tank* curr = m_current->m_next; //curr declared and initialized to m_current->m_next since tankID moved to next
        Pump* pumpCurr = curr->m_pumps; //pumpCurr declared and initialized to m_pumps

        //fuelTransfer declared and initialized
        //if fuel <= tankFuel, then all of the fuel will be transferred over. Else, only the amount in the tank is
        int fuelTransfer = ((fuel <= curr->m_tankFuel) ? fuel : curr->m_tankFuel);

        //while loop goes through the m_pumps linked list and see if the pumpID exists, then takes the pump ID
        while (pumpCurr != nullptr){
            //if statement checks if pumpCurr equals m_pumps
            if (pumpCurr->m_pumpID == pumpID){
                //if attempts to find the m_target tank
                if (findTank(pumpCurr->m_target)){
                    //since findTank changes m_current, m_current->m_next is still the targetTank
                    int startingFuel = m_current->m_next->m_tankFuel; 
                    fill(pumpCurr->m_target, fuelTransfer);

                    //fuelDifference is the current fuel of the target minus the starting fuel
                    int fuelDifference = m_current->m_next->m_tankFuel - startingFuel;

                    //newFuel uses a ternary operator. If the fuelTransfer amount is greater than or equal to the fuel difference,
                    //that is the amount transferred and subtracted from curr. However, if not, the fuel difference is subtracteds
                    int newFuel = ((fuelTransfer <= fuelDifference) ? fuelTransfer : fuelDifference);

                    curr->m_tankFuel -= newFuel; //newFuel amount removed from the current tank
                    return true;
                }
                else{
                    return false;
                }
            }

            //pumpCurr moves up one
            pumpCurr = pumpCurr->m_next;
        }
        //if the while loop goes through the linked list and doesn't find the pumpID, it returns false
        return false;
    }
    else{
        return false;
    }
}


//fill
//It fills up the tank to capacity 
bool FuelSys::fill(int tankID, int fuel){
    //if statement checks to ensure that the list is not empty and fuel sent is greater than zero
    if (findTank(tankID) && (fuel > 0)){
        Tank* curr = m_current->m_next;  //curr declared and initialized as findTank moved it to the next
        
        //int newFuel declared and initialized to a ternary operator
        //if fuel is less than or equal to tankCapacity minus the current fuel in the tank, fuel is used
        //otherwise, it is set to curr->m_tankCapacity - curr->m_tankFuel since it will fill the tank up to capacity
        int newFuel = ((fuel <= (curr->m_tankCapacity - curr->m_tankFuel)) ? fuel : curr->m_tankCapacity - curr->m_tankFuel);
        
        curr->m_tankFuel += newFuel; //newFuel is added to the m_tankFuel
        return true; //returns true if completed
    }
    else{
        return false;
    }
}

//operator=
//Assignment operator which makes the left side equal the right sideghnn
const FuelSys & FuelSys::operator=(const FuelSys & rhs){
    //checks to ensure that it is not self assigning
    if (this != &rhs){
        //the destructor is called to destroy this
        this->~FuelSys();

        //if statement checks that rhs.m_current is not an empty object
        if (rhs.m_current != nullptr){
            Tank* copiedCurr = rhs.m_current; //copiedCurr declared and initialized to m_current
            Tank* thisCurr = m_current; //thisCurr declared
            //if statement checks to ensure that m_pumps does not equal nullptr

            do{
                Tank* newTank; //newTank declared, initialized later

                //if statement checks to ensure that m_pumps does not equal nullptr
                if (copiedCurr->m_pumps != nullptr){
                    //Three Pump pointers declared and initialized
                    //copiedPumpCurr goes through the rhs linked list to copy everything over
                    //newPump creates a new pump object with the data from rhs's m_pump
                    //pumpCurr is used to traverse and add stuff to the new linked list
                    Pump* copiedPumpCurr = copiedCurr->m_pumps;
                    Pump* newPump = new Pump (copiedPumpCurr->m_pumpID, copiedPumpCurr->m_target);
                    Pump* pumpCurr = newPump;

                    //while loop goes through the singly linked list and dynamically allocates each pump Node
                    while (copiedPumpCurr->m_next != nullptr){
                        copiedPumpCurr = copiedPumpCurr->m_next; //copiedPumpCurr moved one ahead
                        
                        //addPump is dynamically allocated with the same data a in copiedPumpCurr
                        Pump* addPump = new Pump (copiedPumpCurr->m_pumpID, copiedPumpCurr->m_target);
                        
                        pumpCurr->m_next = addPump; //pumpCurr's m_next now points to addPump
                        pumpCurr = pumpCurr->m_next; //pumpCurr moves up one
                    }

                    //newTank object initialized and added to the linked list
                    newTank = new Tank(copiedCurr->m_tankID, copiedCurr->m_tankCapacity, copiedCurr->m_tankFuel, newPump);
                }
                else{
                    //else newTank initialized without newPump accounted for
                    newTank = new Tank(copiedCurr->m_tankID, copiedCurr->m_tankCapacity, copiedCurr->m_tankFuel);
                }
                //if statement checks to ensure that thisCurr does not equal nullptr, and if it does the newTank is set to thisCurr
                //this is done to prevent seg faulting
                if (thisCurr == nullptr){
                    thisCurr = newTank;
                    m_current = thisCurr;
                }
                else{
                    thisCurr->m_next = newTank; //m_next set to the new tank object created
                    thisCurr = thisCurr->m_next; //thisCurr moves on
                }

                //copiedcurr moved up one
                copiedCurr = copiedCurr->m_next;
            } while (copiedCurr != rhs.m_current);
            thisCurr->m_next = m_current; //next points to m_current
        }
    }
    return *this; //this is returned
}

//dumpSys
//displays the tanks in the Fuel System
void FuelSys::dumpSys() const{
    if (m_current != nullptr){
        Tank* tempTank = m_current->m_next;//we start at front item
        //we continue visting nodes until we reach the cursor
        while(m_current != nullptr && tempTank != m_current){
            cout << "Tank " << tempTank->m_tankID << "(" << tempTank->m_tankFuel << " kg)" << endl;
            // now dump the targets for all pumps in this tank
            // we need to traverse the list of pumps
            dumpPumps(tempTank->m_pumps);
            tempTank = tempTank->m_next;
        }
        //at the end we visit the cursor (current)
        //this also covers the case that there is only one item
        cout << "Tank " << m_current->m_tankID << "(" << m_current->m_tankFuel << " kg)" << endl;
        dumpPumps(tempTank->m_pumps);
        cout << "The current tank is " << m_current->m_tankID << endl;
    }
    else
        cout << "There is no tank in the system!\n\n";
}

//dumpPumps
//Displays the pumps in the linked list
void FuelSys::dumpPumps(Pump* pumps) const{
    // this traverses the linked list to the end
    Pump* tempPump = pumps;
    while (tempPump != nullptr){
        cout << " => pump " << tempPump->m_pumpID << "(To tank " << tempPump->m_target << ")" << endl;
        tempPump = tempPump->m_next;
    }
}