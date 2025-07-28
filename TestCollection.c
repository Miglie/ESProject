/************************************************************************* */
/*                                                                         */
/*                               TEST ZONE                                 */
/*                                                                         */
/***************************************************************************/

//TEST 1: Basic int test (Single process)

//Basic first test with simple data type to check if first implementation was working
//Since we are using a basic type flag to deallocate memory is set to 0 (not deallocate)
//Expected output: green led is toggled approximately every 1000 ms

/* USER CODE BEGIN PFP */
void basicIntegerTest(void * argument);
int compare(void * first, void * second);
void commit(void * result);
/* USER CODE END PFP */

/* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
xTaskCreate_TMR(basicIntegerTest, "basicIntTest", 512, NULL, 1, NULL);
/* USER CODE END RTOS_THREADS */

/* USER CODE BEGIN 4 */
int compare(void * primo, void * secondo){
  //MEMO: User have to explicitely typecast into the right pointer type
  int * first;
  int * second;
  first = (int *)primo;
  second = (int *)secondo;
  if(*first == *second) return 1;
  return 0;
}

void commit(void *result){
  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
}

void basicIntegerTest(void * argument){
  for(;;){
    int i = 1;
    void * pointer = &i;
    const TickType_t xDelay = 1000/portTICK_PERIOD_MS;
    taskTerminated(pointer, 0, compare, commit, xDelay);
  }
}
/* USER CODE END 4 */

/*************************************************************************************************************/

//TEST 2: Basic struct test (Single process)

//Simple test to check the system behaviour when working with complex, user-defined data types (e.g. TestStruct)
//Expected behaviour: Green led is toggled approximately every 500 ms

//FILE: main.h

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
  int integer;
  float floating;
}TestStruct;
/* USER CODE END ET */

//FILE: main.c

/* USER CODE BEGIN PFP */
void basicStructTest(void * argument);
int compare(void * first, void * second);
void commit(void * result);
/* USER CODE END PFP */

/* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
xTaskCreate_TMR(basicStructTest, "basicStructTest", 512, NULL, 1, NULL);
/* USER CODE END RTOS_THREADS */

/* USER CODE BEGIN 4 */
int compare(void * first, void * second){
    TestStruct * primo;
    TestStruct * secondo;
    primo = (TestStruct*) first;
    secondo = (TestStruct*) second;
    if((primo->integer == secondo->integer)&&(primo->floating == secondo->floating)){
        return 1;
    }
    return 0;
}

void commit(void *result){
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
}

void basicStructTest(void *argument){
  for(;;){
    TestStruct * pointer;
    pointer = pvPortMalloc(sizeof(TestStruct));
    pointer->integer = 5;
    pointer->floating = 1.215;
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    //SARA: Pointer deve essere castato void prima di passarlo a taskTerminated?
    taskTerminated(pointer, 1, compare, commit, xDelay);  
  }
}
/* USER CODE END 4 */

/*************************************************************************************************************/

//TEST 3: Voting test (Single process)

//Simple test to check functioning of voting system. Test relies on a counter that assumes values 0 and 1
//and is toggled everytime its value is used.
//Expected behaviour: leds are toggled in an alternating way, starting with red (majority of 0s) then
//green (majority of 1s)

/* USER CODE BEGIN PV */
int counter = 0;
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
void votingTest(void * argument);
int compare(void * first, void * second);
void commit(void * result);
/* USER CODE END PFP */

/* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
xTaskCreate_TMR(votingTest, "votingTest", 512, NULL, 1, NULL);
/* USER CODE END RTOS_THREADS */

/* USER CODE BEGIN 4 */
int compare(void * primo, void * secondo){
  //MEMO: User have to explicitely typecast into the right pointer type
  int * first;
  int * second;
  first = (int *)primo;
  second = (int *)secondo;
  if(*first == *second) return 1;
  return 0;
}

void commit(void * result){
    int * output;
    output = (int *)result;
    if(*output == 1){
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
    } else if(*output == 0){
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
    }
}

void votingTest(void *argument){
  for(;;){
    int integer;
    int * pointer = &integer;
    if(counter==1){
      * pointer = counter;
      counter--;
    } else if(counter==0){
       * pointer = counter;
      counter++;
    }
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    taskTerminated(pointer, 0, compare, commit, xDelay);
  }
}
/* USER CODE END 4 */

/*************************************************************************************************************/

//TEST 4: Double process test

//Test to check system behaviour when working with more than one task at a time
//Expected output: Processes toggle green and blue LED with different frequencies defined in xDelay

/* USER CODE BEGIN PFP */
void taskTest1(void * argument);
void taskTest2(void * argument);
int compare(void * first, void * second);
void commit1();
void commit2();
/* USER CODE END PFP */

/* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
xTaskCreate_TMR(taskTest1, "taskTest1", 512, NULL, 1, NULL);
xTaskCreate_TMR(taskTest2, "taskTest2", 512, NULL, 1, NULL);
/* USER CODE END RTOS_THREADS */

/* USER CODE BEGIN 4 */
int compare(void * primo, void * secondo){
  int * first;
  int * second;
  first = (int *)primo;
  second = (int *)secondo;
  if(*first == *second) return 1;
  return 0;
}

void commit1(void *result){
  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
}

void commit2(void *result){
  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
}

void taskTest1(void * argument){
  for(;;){
    int i = 1;
    void * pointer = &i;
    const TickType_t xDelay = 1000/portTICK_PERIOD_MS;
    taskTerminated(pointer, 0, compare, commit1, xDelay);
  }
}

void taskTest2(void * argument){
  for(;;){
    int i = 1;
    void * pointer = &i;
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    taskTerminated(pointer, 0, compare, commit2, xDelay);
  }
}
/* USER CODE END 4 */

/*************************************************************************************************************/

//TEST 5: Bitflip + error handling test (Single process)

//Test to check behaviour related to the specifics (redundancy against memory corruption)
//Task code is the same of the first basic test. Using gdb some bits are flipped before output evaluation
//and the effects of the flips can be seen on the action applied by the commit function.
//If all outputs are different the commit function is never called: this case is handled by the task itself
//Expected output: toggle green (majority of 1s), toggle blue(majority of 0s), toggle red (error, all outputs are different)

/* USER CODE BEGIN PFP */
void bitFlipTest(void *argument);
int compare(void * first, void * second);
void commit(void * result);
/* USER CODE END PFP */

/* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
xTaskCreate_TMR(bitFlipTest, "bitFlipTest", 512, NULL, 1, NULL);
/* USER CODE END RTOS_THREADS */

/* USER CODE BEGIN 4 */
int compare(void * primo, void * secondo){
  int * first;
  int * second;
  first = (int *)primo;
  second = (int *)secondo;
  if(*first == *second) return 1;
  return 0;
}

void commit(void *result){
  int *risultato;
  risultato = (int*) result;
  if(*risultato==1){
    //green
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
  } else if (*risultato==0){
    //blue
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
  } 
}

void bitFlipTest(void *argument){
	for(;;){
		int integer;
		int * pointer = &integer;
		* pointer = 1;
		const TickType_t xDelay = 500/portTICK_PERIOD_MS;
		if(taskTerminated(pointer, 0, compare, commit, xDelay)== pdFAIL){
			//red
			 HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		}
	}
}
/* USER CODE END 4 */

/*************************************************************************************************************/

//TEST 6: The FINAL Test 

//Three tasks running, user-defined complex data type, possible bitflip check
//Expected output: Without modification led should light in order green, blue, orange and remain on.
//Red led is switched on when the first error occurs (three different ouputs) and never switched off
//One can play with gdb modifying struct values to switch on/off the pins
//A possible interpretation is a control of some value measured: if the value is the one desired the led
//relative to the task is on, when the value differs from the one desired but the measure is still reliable
//the led is switched off, if there are completely different values the measure is not reliable anymore
//and the error red led is switched on, signaling that there is something wrong with the sensors.

//FILE: main.h

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
  int integer;
  float floating;
}TestStruct;
/* USER CODE END ET */

//FILE: main.c

/* USER CODE BEGIN PFP */
void taskOne(void * argument);
void taskTwo(void * argument);
void taskThree(void * argument);
int compare(void * first, void * second);
void commitOne(void * result);
void commitTwo(void * result);
void commitThree(void * result);
/* USER CODE END PFP */

/* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
xTaskCreate_TMR(taskOne, "taskOne", 512, NULL, 1, NULL);
xTaskCreate_TMR(taskTwo, "taskTwo", 512, NULL, 1, NULL);
xTaskCreate_TMR(taskThree, "taskThree", 512, NULL, 1, NULL);
/* USER CODE END RTOS_THREADS */

/* USER CODE BEGIN 4 */
int compare(void * first, void * second){
    TestStruct * first_o;
    TestStruct * second_o;
    first_o = (TestStruct *)first;
    second_o = (TestStruct *)second;
    if(first_o->integer == second_o->integer && first_o->floating == second_o->floating) return 1;
    return 0;
}

//Green led
void commitOne(void * result){
    TestStruct * result_o;
    result_o = (TestStruct *)result;
    if(result_o->integer == 1 && result_o->floating <= 0.5){
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    }
}

//Blue led
void commitTwo(void * result){
    TestStruct * result_o;
    result_o = (TestStruct *)result;
    if(result_o->integer == 1 && result_o->floating <= 0.5){
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    }
}

//Orange led
void commitThree(void * result){
    TestStruct * result_o;
    result_o = (TestStruct *)result;
    if(result_o->integer == 1 && result_o->floating <= 0.5){
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    }
}

void taskOne(void * argument){
    TestStruct * one;
    one = pvPortMalloc(sizeof(TestStruct));
    one->integer = 1;
    one->floating = 0.5;
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    if(taskTerminated(one, 1, compare, commitOne, xDelay) == pdFAIL){
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    }
}
void taskTwo(void * argument){
    TestStruct * two;
    two = pvPortMalloc(sizeof(TestStruct));
    two->integer = 1;
    two->floating = 0.5;
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    if(taskTerminated(two, 1, compare, commitTwo, xDelay) == pdFAIL){
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    }
}
void taskThree(void * argument){
    TestStruct * three;
    three = pvPortMalloc(sizeof(TestStruct));
    three->integer = 1;
    three->floating = 0.5;
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    if(taskTerminated(three, 1, compare, commitThree, xDelay) == pdFAIL){
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    }
}
/* USER CODE END 4 */

