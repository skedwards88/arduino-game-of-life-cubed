Immediate neighbors  -  (vary just x, y, or z)



Conditional trio  -  cube state random (1, 4)  -  cycle time 250ms
Note: sometimes freezes after many cycles
 if (currentValue == 1 && (numNeighborsWithState2) == 1)
 {
   return 2;
 }
 if (currentValue == 2 && (numNeighborsWithState3) == 1)
 {
   return 3;
 }
 if (currentValue == 3 && (numNeighborsWithState1) == 1)
 {
   return 1;
 }
 else
 {
   return currentValue;
 }

Empty space duo  -  cube state random (0, 3)  -  cycle time 1000ms
 if (currentValue == 0 && (numNeighborsWithState1) == 1)
 {
   return 1;
 }
   if (currentValue == 1 && (numNeighborsWithState1) == 2)
 {
   return 2;
 }
   if (currentValue == 0 && (numNeighborsWithState2) == 1)
 {
   return 2;
 }
   if (currentValue == 2 &&(numNeighborsWithState2) == 2)
 {
   return 1;
 }
 else
 {
   return 0;
 }

Empty space solo  -  cube state random (0, 1)  -  cycle time 200ms
Note: eventually freezes or goes into simple cycles
 if (currentValue == 0 && (numNeighborsWithState1) > 1)
 {
   return 1;
 }
 if (currentValue == 1 && (numNeighborsWithState1) > 1)
 {
   return 0;
 }
 else
 {
   return currentValue;
 }

Asymmetric duo  - cube state random (1, 3)  -  cycle time 500ms
Note: eventually freezes or goes into simple cycles
 if (currentValue == 2 && (numNeighborsWithState1) > 1)
 {
   return 1;
 }
 if (currentValue == 1 && (numNeighborsWithState1) > 1)
 {
   return 2;
 }
 else
 {
   return currentValue;
 }


Succession  -  cube state random (1, 3)  -  cycle time 1000ms
if (currentValue == 2 && (numNeighborsWithState1) > 0)
 {
   return 1;
 }
 if (currentValue == 1 && (numNeighborsWithState1) > 3)
 {
   return 0;
 }
   if (currentValue == 1 && (numNeighborsWithState0) > 3)
 {
   return 0;
 }
   if (currentValue == 0 && (numNeighborsWithState2) > 0)
 {
   return 2;
 }
     if (currentValue == 0 && (numNeighborsWithState0) > 4)
 {
   return 2;
 }
       if (currentValue == 2 && (numNeighborsWithState2) > 5)
 {
   return random(1, 3);
 }
 else
 {
   return currentValue;
 }



Diagonal and immediate neighbors  -  (vary just x, y, or z or two of each)

Succession  -  cube state random (1, 3)  -  cycle time 1000ms
Note: eventually blue or red wins out, or stabilizes
if (currentValue == 2 && (numNeighborsWithState1) > 1)
 {
   return 1;
 }
 if (currentValue == 1 && (numNeighborsWithState1) > 4)
 {
   return 0;
 }
   if (currentValue == 0 && (numNeighborsWithState2) > 0)
 {
   return 2;
 }
     if (currentValue == 0 && (numNeighborsWithState0) > 15)
 {
   return 2;
 }
       if (currentValue == 2 && (numNeighborsWithState2) > 18)
 {
   return random(1, 3);
 }
 else
 {
   return currentValue;
 }





Single Start Spot

Diagonal Waves  -  cycle time 800ms

 // todocolin -- can be:
 // 0 = random seeding for all spots
 // 1 = top left is random color, all others off
 // 2 = single spot is random color, all others off
 const int START_MODE = 1;
 // todocolin--can change the initial color here. random() is min inclusive, max exclusive
 int randomMin = 1;
 int randomMax = 2;

// todocolin--can omit some directions if desired
const int DIRECTIONS[][3] = {
   // Vary just x, y, or z
   {1, 0, 0},
   {0, 1, 0},
   {0, 0, 1},
   //Vary xy, xz, or yz
   // {1, 1, 0},
   // {1, 0, 1},
   // {0, 1, 1},
   // {1, -1, 0},
   // {1, 0, -1},
   // {0, 1, -1},
   // Vary xyz
   // {1, 1, 1},
   // {1, -1, 1},
   // {1, 1, -1},
   // {-1, 1, 1}

// todocolin--can change the conditions for life here
 // 0 = off, 1 = Color 1, 2 = Color 2, 3 = Color 1+2
 // numSteps can be set
 // Primary rules
 if (currentValue == 0 && (numNeighborsWithState1) > 0)
 {
   return 1;
 }
 else if (currentValue == 1 && (numNeighborsWithState2) > 0)
 {
   return 2;
 }
 else if (currentValue == 2 && (numNeighborsWithState3) > 0)
 {
   return 3;
 }
 else if (currentValue == 3 && (numNeighborsWithState0) > 0)
 {
   return 0;
 }
 else if (currentValue == 1 && (numNeighborsWithState1)== 3)
 {
   return 2;
 }
   else if (currentValue == 2 && (numNeighborsWithState2) == 3)
 {
   return 3;
 }
   else if (currentValue == 3 && (numNeighborsWithState3) == 3)
 {
   return 0;
 }
  else if (currentValue == 0 && (numNeighborsWithState0) == 3 && numSteps > 6)
 {
   return 1;
 }
     else if (currentValue == 0 && (numNeighborsWithState0) == 3)
 {
   return 0;
 }
 else
 {
   return currentValue;
 }




Tendrils  -  cube state random (1, 2)  -  cycle time 1200ms

 // todocolin -- can be:
 // 0 = random seeding for all spots
 // 1 = top left is random color, all others off
 // 2 = single spot is random color, all others off
 const int START_MODE = 2;
 // todocolin--can change the initial color here. random() is min inclusive, max exclusive
 int randomMin = 1;
 int randomMax = 2;

// todocolin--can omit some directions if desired
const int DIRECTIONS[][3] = {
   // Vary just x, y, or z
   {1, 0, 0},
   {0, 1, 0},
   {0, 0, 1},
   //Vary xy, xz, or yz
   // {1, 1, 0},
   // {1, 0, 1},
   // {0, 1, 1},
   // {1, -1, 0},
   // {1, 0, -1},
   // {0, 1, -1},
   // Vary xyz
   // {1, 1, 1},
   // {1, -1, 1},
   // {1, 1, -1},
   // {-1, 1, 1}
};



// todocolin--can change the conditions for life here
 // 0 = off, 1 = Color 1, 2 = Color 2, 3 = Color 1+2
 // numSteps can be set
 // Primary rules
 if (currentValue == 0 && (numNeighborsWithState1) == 1)
 {
   return random(0,2);
 }
 else if (currentValue == 1)
 {
   return 3;
 }
 else if (currentValue == 3)
 {
   return 2;
 }
 else if (currentValue == 3 && (numNeighborsWithState0) > 0)
 {
   return 0;
 }
  else if (currentValue == 2 && (numNeighborsWithState2 + numNeighborsWithState0) > 2 && (numNeighborsWithState1 + numNeighborsWithState3) == 0)
 {
   return 0;
 }
    else if (currentValue == 0 && (numNeighborsWithState0) > 5 && numSteps > 12)
 {
   return random (0,2);
 }
 else
 {
   return currentValue;
 }



Space Battle - cycle time 800 ms

Red "shot" propagates as purple and turns blue if it hits the other side (if it doesn't run into another shot).

   if (currentValue == 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 1 && (numNeighborsWithState3) == 1 && numSteps%4 ==0)
 {
   return 2; // corner/edge first propagates purple if present
 }
   else if (currentValue == 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 1 && (numSteps - 1)%4 == 0)
 {
   return random(0, 2); // otherwise, corner/edge randomly "sparks" red 1/2 of time on every 3th step if no purples propagating adjacent
 }
   else if (currentValue == 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 2 && (numNeighborsWithState1 + numNeighborsWithState3) == 1 && numSteps%4 != 0)
 {
   return 3; // red sparks propagate as purple, purple continues to propagate
 }
   else
 {
   return 0; // default is blank, not currentValue
 }

// todocolin--can omit some directions if desired
const int DIRECTIONS[][3] = {
   // Vary just x, y, or z
   {1, 0, 0},
   // {0, 1, 0},
   // {0, 0, 1},
   //Vary xy, xz, or yz
   // {1, 1, 0},
   // // {1, 0, 1},
   // // {0, 1, 1},
   // {1, -1, 0},
   // // {1, 0, -1},
   // // {0, 1, -1},
   // Vary xyz
   // {1, 1, 1},
   // {1, -1, 1},
   // {1, 1, -1},
   // {-1, 1, 1}

 // todocolin -- can be:
 // 0 = random seeding for all spots
 // 1 = top left is random color, all others off
 // 2 = single spot is random color, all others off
 const int START_MODE = 0;
 // todocolin--can change the initial color here. random() is min inclusive, max exclusive
 int randomMin = 0;
 int randomMax = 1;


Snake (in progress)

1 corner starts as red
 if (currentValue == 1)
   {return 2;}  // purple head turns to blue

 else if (currentValue == 0 && numNeighborsWithState1 > 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 4 && (numSteps+6)%7 ==0)
   {return random(0,2);}  // at second step in cycle, head moves to edge spot(s) randomly
  else if (currentValue == 0 && numNeighborsWithState1 > 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 5 && (numSteps+5)%7 ==0)
   {return 1;}  // at first step in cycle, head moves to middle-side spot if able


 else if (currentValue == 0 && numNeighborsWithState1 > 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 6 && (numSteps+4)%7 ==0)
   {return 1;}  // at first step in cycle, head moves to center spot if able


 else if (currentValue == 0 && numNeighborsWithState1 > 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 6 && (numSteps+3)%7 ==0)
   {return 1;}  // at first step in cycle, head moves to center spot if able


 else if (currentValue == 0 && numNeighborsWithState1 > 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 5 && (numSteps+2)%7 ==0)
   {return 1;}  // at first step in cycle, head moves to middle-side spot if able


 else if (currentValue == 0 && numNeighborsWithState1 > 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 4 && (numSteps+1)%7 ==0)
   {return 1;}  // at second step in cycle, head moves to edge spot(s) randomly


 else if (currentValue == 0 && numNeighborsWithState1 > 0 && (numNeighborsWithState0 + numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) == 3 && (numSteps)%7 ==0)
   {return 1;}  // at second step in cycle, head moves to corner spot if able


   {return currentValue;} // default is blank, not currentValue
}

Da Code!!!
