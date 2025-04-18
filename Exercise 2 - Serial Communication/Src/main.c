#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stm32f303xc.h"
#include "movement.h"
#include "serial.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


// visual delay after transmission
void finished_transmission(uint32_t bytes_sent) {
    for (volatile uint32_t i = 0; i < 0x3ffff; i++) { }
}

//Declaring constants
uint8_t *string_to_send = "I'm on a poll!\r\n";
const int NUMBER_OF_SHAPES = 16;

void ShapeDemo();
int is_start_command(char *input);//  Function to check if input is the command "start"


int main(void)
{
	SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);
	setupNVIC();

//	/* Loop forever */
	while(1){
		__WFI();
		if (start_flag == 1){
			ShapeDemo();
	}
	}
}




void ShapeDemo(void) {

	struct shape shape_1 = generate_shape(SHAPE_SPHERE, 5, 5);
	struct shape shape_2;
	struct shape *shape_3;

	struct shape shape_array[16];

	uint8_t string_buffer[64];

	shape_string(&shape_1, &string_buffer[0]);
	SerialOutputStringDB(&string_buffer[0], &USART1_PORT);

	shape_string(&shape_2, &string_buffer[0]);
	SerialOutputStringDB(&string_buffer[0], &USART1_PORT);

	shape_3 = (struct shape*)malloc(sizeof(struct shape));
	shape_string(shape_3, &string_buffer[0]);
	SerialOutputStringDB(&string_buffer[0], &USART1_PORT);

	*shape_3 = generate_shape(SHAPE_CONE, 6,3);
	shape_string(shape_3, &string_buffer[0]);
	SerialOutputStringDB(&string_buffer[0], &USART1_PORT);


	for (uint32_t counter = 0; counter < NUMBER_OF_SHAPES; counter++) {
		if (initialise_shape(&shape_array[counter], counter % 3, 3, 4)) {
			shape_string(&shape_array[counter], &string_buffer[0]);
			SerialOutputStringDB(&string_buffer[0], &USART1_PORT);
		} else {
			shape_string(&shape_array[counter], &string_buffer[0]);
			SerialOutputStringDB(&string_buffer[0], &USART1_PORT);
		}
	}

	// slide the shapes
	sprintf(&string_buffer[0], "\nsliding the shapes by 1.5 units\r");
	SerialOutputStringDB(&string_buffer[0], &USART1_PORT);

	for (uint32_t counter = 0; counter < NUMBER_OF_SHAPES; counter++) {
		slide_shape(&shape_array[counter], 1.5);
		shape_string(&shape_array[counter], &string_buffer[0]);
		SerialOutputStringDB(&string_buffer[0], &USART1_PORT);
	}

	// roll the shapes
	sprintf(&string_buffer[0], "\nrolling the shapes by 2.5 units\n");
	SerialOutputStringDB(&string_buffer[0], &USART1_PORT);
	for (uint32_t counter = 0; counter < NUMBER_OF_SHAPES; counter++) {
		roll_shape(&shape_array[counter], 2.5);
		shape_string(&shape_array[counter], &string_buffer[0]);
		SerialOutputStringDB(&string_buffer[0], &USART1_PORT);
	}

	for(;;) {
		SerialOutputStringDB(string_to_send, &USART1_PORT);
	}
}





