#include "main.h"
using namespace pros;

Controller controls(E_CONTROLLER_MASTER);

Motor front_left(1, E_MOTOR_GEARSET_36, true, E_MOTOR_ENCODER_DEGREES);
Motor mid_left(2, E_MOTOR_GEARSET_36, false, E_MOTOR_ENCODER_DEGREES);
Motor back_left(3, E_MOTOR_GEARSET_36, true, E_MOTOR_ENCODER_DEGREES);

Motor front_right(4, E_MOTOR_GEARSET_36, false, E_MOTOR_ENCODER_DEGREES);
Motor mid_right(5, E_MOTOR_GEARSET_36, true, E_MOTOR_ENCODER_DEGREES);
Motor back_right(6, E_MOTOR_GEARSET_36, false, E_MOTOR_ENCODER_DEGREES);

Motor_Group left_motors({ front_left, mid_left, back_left });
Motor_Group right_motors({ front_right, mid_right, back_right });

void initialize() {
	lcd::initialize();
	lcd::set_text(1, "Screen working");
	controls.set_text(0, 0, "Controller Working");
}

void disabled() {}
void competition_initialize() {}
void autonomous() {}
void opcontrol() {

	int x1, y1, x2, y2 = 0;
	int left_speed = 0, right_speed = 0;
	std::string drive_states[] = { "Double Arcade","Swapped Arcade","Tank","Diff Move" };
	int drive_timer = int(millis()), cur_timer = int(millis());
	int drive_state_num = 2;
	bool exponential_drive = false;
	double power = 1.12;
	int lineNum = 0;
	bool turning_wrong = false;
	bool front_wrong = false;
	while (true) {
		lcd::set_text(lineNum++, "Drive state " + drive_states[drive_state_num] + " Num: " + std::to_string(drive_state_num));

		x1 = controls.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		y1 = controls.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);

		x2 = controls.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		y2 = controls.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y);
		lcd::set_text(lineNum++, "Left: x1 = " + std::to_string(x1) + ", y1= " + std::to_string(y1));
		lcd::set_text(lineNum++, "Right: x2 = " + std::to_string(x2) + ", y2= " + std::to_string(y2));
		if (turning_wrong) {
			x1 *= -1;
			x2 *= -1;
		}
		if (front_wrong) {
			y1 *= -1;
			y2 *= -1;
		}
		switch (drive_state_num) {
		case -1:
			drive_state_num = 4;
			break;
		case 0:
			left_speed = y1 - y2 + x2 + x1;
			right_speed = y1 - y2 - x2 - x1;
			break;
		case 1:
			left_speed = y2 - y1 + x1 + x2;
			right_speed = y2 - y1 - x1 - x2;
			break;
		case 2:
			left_speed = y1;
			right_speed = y2;
			break;
		case 3:
			left_speed = y1 - x2;
			right_speed = y1 + x2;
			break;
		case 4:
			drive_state_num = 0;
			break;
		}

		if (exponential_drive) {
			lcd::set_text(lineNum++, "Exponential true");
			lcd::set_text(lineNum++, "Prev: Right = " + std::to_string(right_speed));
			lcd::set_text(lineNum++, "Prev: Left = " + std::to_string(left_speed));
			left_speed = pow((abs(left_speed) / 127.0), power) * 127.0 * (left_speed / abs(left_speed));
			right_speed = pow((abs(right_speed) / 127.0), power) * 127.0 * (right_speed / abs(right_speed));
		}
		else {
			lcd::set_text(lineNum++, "Exponential false");
		}
		lcd::set_text(lineNum++, "Speeds: Right = " + std::to_string(right_speed) + ", Left = " + std::to_string(left_speed));
		left_motors = left_speed;
		right_motors = right_speed;

		cur_timer = int(millis());
		if (controls.get_digital(E_CONTROLLER_DIGITAL_L1) && drive_timer < cur_timer) {
			drive_state_num++;
			drive_timer = cur_timer + 300;
		}
		if (controls.get_digital(E_CONTROLLER_DIGITAL_L2) && drive_timer < cur_timer) {
			drive_state_num--;
			drive_timer = cur_timer + 300;
		}

		if (controls.get_digital(E_CONTROLLER_DIGITAL_UP)) {
			exponential_drive = true;
		}
		if (controls.get_digital(E_CONTROLLER_DIGITAL_DOWN)) {
			exponential_drive = false;
		}

		if (controls.get_digital(E_CONTROLLER_DIGITAL_LEFT)) {
			power -= 0.01;
		}
		if (controls.get_digital(E_CONTROLLER_DIGITAL_RIGHT)) {
			power += 0.01;
		}
		lcd::set_text(lineNum++, "Exponent: " + std::to_string(power));

		if (controls.get_digital(E_CONTROLLER_DIGITAL_A)) {
			turning_wrong = true;
		}
		if (controls.get_digital(E_CONTROLLER_DIGITAL_B)) {
			turning_wrong = false;
		}

		if (controls.get_digital(E_CONTROLLER_DIGITAL_X)) {
			front_wrong = true;
		}
		if (controls.get_digital(E_CONTROLLER_DIGITAL_Y)) {
			front_wrong = false;
		}

		lineNum = 0;
		pros::delay(60);
	}
}
//Consider making wings a hold as well
/*
R1: Intake Rev
R2: Intake Off
L1: Cata
L2: Wings

Face buttons for blocker/endgame

*/
