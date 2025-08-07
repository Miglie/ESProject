Modified version of the FreeRTOS kernel so that it implements triple modular redundancy of tasks.

To use the triple modular rendundancy the user needs to call xTaskCreate_TMR instead of the classic xTaskCreate and pass the same parameters.
As for the classic xTaskCreate, the user needs to define a function for the task but with some differences.
The part of the code that uses the computation to output something needs to be put in a separate function called commit. It is also necessary to define a compare function that compares the outputs and varies based on the output type. 
At the end of the task function the user needs to call the function taskTerminated and pass as parameters the output, commit, compare, the delay and deallocate_memory flag. In case the user defined new types or structs the flag needs to be put to 1, otherwise if a memory allocation is not possible or if the data structure has to be preserved the flag has to be put to 0. 

Some examples of the usage of the functions are available in the file TestCollection.c. More details on the implementation are available as comments in tasks.c

The STM Cube project that is on the repository is for STM32F407. If one wants to use the kernel on another microcontroller, just download the FreeRTOS files.

Project developed for the Embedded Systems and Advanced Operating Systems courses of Politecnico di Milano.
