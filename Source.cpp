#include <Windows.h>
#include <pthread.h>
#include <CImg.h>

using namespace cimg_library;



#define byte unsigned char
#define ush  unsigned short
#define uint unsigned int
#define ll   long long
#define ull  unsigned long long

#define pi 3.141592

const byte black[3] = { 0, 0, 0 };
const byte white[3] = { 255, 255, 255 };

#define image CImg<unsigned char>

#define XSize 1560
#define YSize 800
#define AnimateVictoryScreen 0
#define VictoryScreenAnimationDelay 10



bool ProgramIsRunning = 1;
bool Wait = 0;
bool LMBISPressed = 0;  // lmb is lwft mouse button
bool BackToMainMenu = 0;

image img(XSize, YSize, 1, 3);
CImgDisplay display(img);

byte number_of_dimentions = 2;
byte number_of_players = 2;



void* input_box_changer(void* raw);
class inputbox {
public:
	const byte triangle_color[3] = { 255, 255, 64 };
	const byte text_bg_color[3] = { 192, 192, 192 };
	const byte text_fg_color[3] = { 0, 0, 0 };
	const byte text_font = 48;
	const byte distance = 10;
	ush width = text_font * 9 / 7;

	ush xcor;
	ush ycor;
	byte minval, maxval;
	byte* value;
	bool display_this_inputbox = 1;
	bool horizontal;

	pthread_t* thread_pointer;

	inputbox(ush newxcor, ush newycor, byte* newvalue, byte new_minimal_value, byte new_maximal_value, bool is_horizontal) {
		xcor = newxcor;
		ycor = newycor;
		minval = new_minimal_value;
		maxval = new_maximal_value;
		value = newvalue;
		horizontal = is_horizontal;

		draw();

		thread_pointer = new pthread_t;
		pthread_create(thread_pointer, 0, &input_box_changer, this);
	}
	bool draw() {
		try {
			Sleep(10);

			if (!display_this_inputbox) {
				return 0;
			}

			CImg<ush> coords(3, 2);
			if (horizontal) {
				coords(0, 0) = xcor + width;
				coords(0, 1) = ycor;
				coords(1, 0) = xcor + width;
				coords(1, 1) = ycor + width;
				coords(2, 0) = xcor;
				coords(2, 1) = ycor + width / 2;

				img.draw_polygon(coords, triangle_color, 1);
				for (byte point = 0; point < 3; point++) {
					coords(point, 0) = xcor * 2 + distance * 2 + width * 3 / 2 + text_font * 2 - coords(point, 0);
				}
				img.draw_polygon(coords, triangle_color, 1);
			}
			else {
				coords(0, 0) = xcor + width / 2;
				coords(0, 1) = ycor;
				coords(1, 0) = xcor;
				coords(1, 1) = ycor + width;
				coords(2, 0) = xcor + width;
				coords(2, 1) = ycor + width;

				img.draw_polygon(coords, triangle_color, 1);
				for (byte point = 0; point < 3; point++) {
					coords(point, 1) = ycor * 2 - coords(point, 1) + width * 2 + distance * 2 + text_font;
				}
				img.draw_polygon(coords, triangle_color, 1);
			}

			char text[4];
			text[3] = '\0';
			ush divider = 1;
			for (byte character = 0; character < 3; character++) {
				text[2 - character] = (((*value) / divider) % 10) + '0';
				divider *= 10;
			}

			if (horizontal) {
				img.draw_text(
					xcor + width + distance, ycor + (width - text_font) / 2,
					text,
					text_fg_color, text_bg_color,
					1, text_font
				);
			}
			else {
				img.draw_text(
					xcor, ycor + width + distance,
					text,
					text_fg_color, text_bg_color,
					1, text_font
				);
			}

			img.display(display);

			return 1;
		}
		catch (...) {
			return 0;
		}
	}
	void clearmemory() {
		pthread_cancel(*thread_pointer);
		delete thread_pointer;

		delete this;
	}
};
void* input_box_changer(void* raw) {
	inputbox* box = static_cast<inputbox*>(raw);

	while (ProgramIsRunning) {
		while (!LMBISPressed) {
			Sleep(10);
		}

		if (box->display_this_inputbox) {
			if (box->horizontal) {
				if ((abs(box->ycor + box->width / 2 - display.mouse_y()) < (box->xcor + (box->distance) * 2 + (box->width) * 2 + (box->text_font) * 3 / 2) / 2 + -display.mouse_x() / 2) &
					(display.mouse_x() >= (box->xcor) + (box->width) + (box->distance) * 2 + (box->text_font) * 4 / 3)) {

					if (*(box->value) < (box->maxval)) {
						(*box->value)++;
					}
				}
				if ((abs(box->ycor + box->width / 2 - display.mouse_y()) < display.mouse_x() / 2 - (box->xcor) / 2) &
					(display.mouse_x() <= box->xcor + box->width)) {

					if (*(box->value) > (box->minval)) {
						(*box->value)--;
					}
				}
			}
			else {
				if ((abs(box->xcor + box->width / 2 - display.mouse_x()) < display.mouse_y() / 2 - (box->ycor) / 2) &
					(display.mouse_y() <= box->ycor + box->width)) {

					if (*(box->value) < (box->maxval)) {
						(*box->value)++;
					}
				}
				if ((abs(box->xcor + box->width / 2 - display.mouse_x()) < ((box->ycor + box->distance * 2 + box->width * 2 + box->text_font - display.mouse_y()) / 2)) &
					(display.mouse_y() >= box->ycor + box->width + box->distance * 2 + box->text_font)) {

					if (*(box->value) > (box->minval)) {
						(*box->value)--;
					}
				}
			}

			if (uint(box->value) != 0xDDDDDDDD) {
				if (!box->draw()) {
					return 0;
				}
			}

			Sleep(250);
		}

		Sleep(10);
	}

	return 0;
}
inputbox* startmenu_inputboxes[2];



void* track_backtomainmenubutton(void* unneeded_arg) {
	while (ProgramIsRunning) {
		if (LMBISPressed) {
			if (display.mouse_y() >= YSize - 128) {
				BackToMainMenu = 1;
			}
		}
		Sleep(10);
	}

	return 0;
}
class Field {
private:
	inputbox** dimention_inputboxes;
	byte* hd_coordinates;  // hd is higher dimention
	byte* tiles;
	const ush indentation = 48;

	byte* getvalue(byte xcor, byte ycor, byte* hdc) {  // hdc is higher - dimentional coordinates
		ull index = 0;
		ull multiplier = 1;
		for (byte dim = 0; dim < number_of_dimentions; dim++) {
			if (dim == 0) {
				index += xcor * multiplier;
			}
			else if (dim == 1) {
				index += ycor * multiplier;
			}
			else {
				index += hdc[dim - 2] * multiplier;
			}
			multiplier *= 3;
		}
		return (tiles + index);
	}
	byte* getvalue(byte* coords) {
		ull index = 0;
		ull multiplier = 1;
		for (byte dim = 0; dim < number_of_dimentions; dim++) {
			index += coords[dim] * multiplier;
			multiplier *= 3;
		}
		return (tiles + index);
	}
	void pick_start_coords(char* changing_vector, byte* out) {  // belongs to byte* countlines() function
		byte start_coords[6];
		byte curr_coords[6];

		for (byte dim = 0; dim < 6; dim++) {
			if (changing_vector[dim] == -1) {
				start_coords[dim] = 2;
			}
			else {
				start_coords[dim] = 0;
			}
		} // setting default start_coordinates
		while (1) {
			if ((*getvalue(start_coords)) != 0) {
				bool brk = 1;
				for (byte dim = 0; dim < number_of_dimentions; dim++) {
					if (changing_vector[dim] != 0) {
						brk = 0;
					}
				}
				if (brk) {
					break;
				}

				for (byte dim = 0; dim < number_of_dimentions; dim++) {
					curr_coords[dim] = start_coords[dim];
				}
				bool three_in_raw = 1;
				for (byte range = 1; range < 3; range++) {
					for (byte dim = 0; dim < number_of_dimentions; dim++) {
						curr_coords[dim] += changing_vector[dim];
					}
					if ((*getvalue(curr_coords)) != (*getvalue(start_coords))) {
						three_in_raw = 0;
					}
				}
				if (three_in_raw) {
					out[*getvalue(start_coords) - 1]++;
				}
			}

			byte leftover;
			leftover = 1;
			for (byte dim = 0; (dim < number_of_dimentions) & leftover; dim++) {
				if (changing_vector[dim] == 0) {
					start_coords[dim] += leftover;
					leftover = start_coords[dim] / 3;
					start_coords[dim] = start_coords[dim] % 3;
				}
			}
			if (leftover) {
				break;
			}
		}  // picking different start_coordinates
	}

public:
	Field(byte dimentions) {
		ull array_size = 1;
		for (byte dim = 0; dim < dimentions; dim++) {
			array_size *= 3;
		}
		tiles = new byte[array_size];
		for (ull tile = 0; tile < array_size; tile++) {
			tiles[tile] = 0;
		}

		if (dimentions > 2) {
			hd_coordinates = new byte[dimentions - 2];
			dimention_inputboxes = new inputbox*[dimentions - 2];
			for (byte dim = 0; dim < dimentions - 2; dim++) {
				hd_coordinates[dim] = 0;
				dimention_inputboxes[dim] = new inputbox(XSize - 300, 25 + 98 * dim, hd_coordinates + dim, 0, 2, 1);
			}
		}

		draw();
	}
	void draw_tiles() {
		ush field_size = YSize - 128 - indentation * 2;
		ush tile_size = field_size / 3;
		ush xdistance = (XSize - field_size - indentation * 2 - 386) / 2;  // 386 is the size of the rectangle on the left side of the screen

		for (byte line_id = 0; line_id <= 3; line_id++) {
			img.draw_line(
				xdistance + indentation, indentation + tile_size * line_id,
				xdistance + indentation + field_size, indentation + tile_size * line_id,
				black,
				1
			);
			img.draw_line(
				xdistance + indentation + tile_size * line_id, indentation,
				xdistance + indentation + tile_size * line_id, indentation + field_size,
				black,
				1
			);
		}  // drawing the frame

		char text[4];
		for (byte tile_xcor = 0; tile_xcor < 3; tile_xcor++) {
			for (byte tile_ycor = 0; tile_ycor < 3; tile_ycor++) {
				if (*getvalue(tile_xcor, tile_ycor, hd_coordinates) != 0) {  // if the tile is not empty
					text[3] = '\0';
					ush divider = 1;
					for (byte character = 0; character < 3; character++) {
						text[2 - character] = (((*getvalue(tile_xcor, tile_ycor, hd_coordinates)) / divider) % 10) + '0';
						divider *= 10;
					}

					img.draw_text(
						tile_size * tile_xcor + tile_size / 2 - tile_size * 3 / 8 + indentation + xdistance,
						tile_size * tile_ycor + tile_size / 4 + indentation,
						text, black, white,
						1, tile_size / 2
					);
				}
				else {
					img.draw_rectangle(
						xdistance + indentation + tile_xcor * tile_size + 1, indentation + tile_ycor * tile_size + 1,
						xdistance + indentation + (tile_xcor + 1) * tile_size - 1, indentation + (tile_ycor + 1) * tile_size - 1,
						white, 1
					);
				}
			}
		}  // filling the insides of the frame

		draw_scores();

		img.display(display);
	}
	void draw_scores() {
		const byte bgcolor[3] = { 196, 196, 196 };

		byte* scores = countlines();
		char text[4];
		text[3] = '\0';
		ush divider;
		img.draw_rectangle(0, 0, 196, YSize - 128, bgcolor, 1);
		img.draw_text(24, 24, "Scores:", black, bgcolor, 1, 48);
		for (byte player = 0; player < number_of_players; player++) {
			divider = 1;
			for (byte character = 0; character < 3; character++) {
				text[2 - character] = ('0' + ((scores[player] / divider) % 10));
				divider *= 10;
			}
			img.draw_text(24, 72 + player * 48, text, black, bgcolor, 1, 48);
		}
		delete scores;

		img.draw_text(24, 72 + (number_of_players + 1) * 48, "From", black, bgcolor, 1, 48);
		divider = 1;
		for (byte character = 0; character < 3; character++) {
			text[2 - character] = ('0' + ((int(pow(3, number_of_dimentions - 2)) / divider) % 10));
			divider *= 10;
		}
		img.draw_text(24, 72 + (number_of_players + 2) * 48, text, black, bgcolor, 1, 48);

		img.draw_line(196, 0, 196, YSize - 128, black, 1);
		img.draw_line(0, YSize - 128, 196, YSize - 128, black, 1);
	}
	void draw() {
		const byte bgcolor[3] = { 196, 196, 196 };

		img.draw_rectangle(XSize - 386, 0, XSize, YSize, bgcolor, 1);
		for (byte dim = 0; dim < number_of_dimentions - 2; dim++) {
			dimention_inputboxes[dim]->draw();
		}

		draw_scores();

		const byte back_button_color[3] = { 208, 64, 64 };
		img.draw_rectangle(0, YSize - 128, XSize - 386, YSize, back_button_color, 1);
		img.draw_text(20, YSize - 108, "Back to main menu", black, back_button_color, 1, 88);
		pthread_t backtomenubutton;
		pthread_create(&backtomenubutton, 0, track_backtomainmenubutton, 0);

		img.draw_line(XSize - 386, 0, XSize - 386, YSize, black, 1);
		img.draw_line(0, YSize - 128, XSize - 386, YSize - 128, black, 1);

		draw_tiles();

		img.display(display);
	}
	void set_inputbox_displaying(bool newvalue) {
		for (byte dim = 0; dim < number_of_dimentions - 2; dim++) {
			dimention_inputboxes[dim]->display_this_inputbox = newvalue;
		}
	}
	void cleanmemory() {
		delete tiles;
		delete hd_coordinates;
		for (byte dim = 0; dim < number_of_dimentions - 2; dim++) {
			dimention_inputboxes[dim]->clearmemory();
		}
	}
	bool sendclick(ush click_xcor, ush click_ycor, byte current_player) {
		ush field_size = YSize - 128 - indentation * 2;  // 128 is the size of "Back to main menu" button
		ush tile_size = field_size / 3;
		ush xdistance = (XSize - field_size - indentation * 2 - 386) / 2;  // 386 is the size of the rectangle on the left side of the screen


		for (byte line_xcor = 0; line_xcor < 3; line_xcor++) {
			for (byte line_ycor = 0; line_ycor < 3; line_ycor++) {
				if (
					((click_xcor > xdistance + indentation + tile_size * line_xcor) & (click_xcor < xdistance + indentation + tile_size * (line_xcor + 1)))
					&
					((click_ycor > indentation + tile_size * line_ycor) & (click_ycor < indentation + tile_size * (line_ycor + 1)))
					)
				{
					if (*getvalue(line_xcor, line_ycor, hd_coordinates) == 0) {  // if the tile is empty
						*getvalue(line_xcor, line_ycor, hd_coordinates) = current_player + 1;
						draw_tiles();

						return 1;
					}
					else {
						return 0;
					}
				}
			}
		}

		return 0;
	}
	bool check_for_fullness() {
		ull array_size = 1;
		for (byte dim = 0; dim < number_of_dimentions; dim++) {
			array_size *= 3;
		}
		for (ull ind = 0; ind < array_size; ind++) {
			if (tiles[ind] == 0) {
				return 0;
			}
		}

		return 1;
	}
	byte* countlines() {
		byte* out = new byte[254];
		for (byte player = 0; player < 254; player++) {
			out[player] = 0;
		}

		char changing_vector[6];
		for (byte dim = 0; dim < number_of_dimentions; dim++) {
			changing_vector[dim] = -1;
		}

		bool cv_change = 1;  // cv is changing_vector
		ull number_of_iterations = (pow(3, number_of_dimentions) - 1) / 2;
		for (ull iteration = 0; iteration < number_of_iterations; iteration++) {
			pick_start_coords(changing_vector, out);

			changing_vector[0]++;
			for (byte dim = 0; dim < number_of_dimentions; dim++) {
				if (changing_vector[dim] > 1) {
					if (dim < number_of_dimentions - 1) {
						changing_vector[dim + 1]++;
						changing_vector[dim] = -1;
					}
					else {
						cv_change = 0;
						break;
					}
				}
			}  // changing changing_vector
		}

		return out;
	}
	byte findwinner() {
		byte* number_of_lines = countlines();

		bool winners[254];
		for (byte ind = 0; ind < 254; ind++) {
			winners[ind] = 0;
		}
		if (check_for_fullness()) {
			byte maximal = 0;
			for (bool new_maximal_found = 1; new_maximal_found; new_maximal_found = 0) {
				for (byte player = 0; player < number_of_players; player++) {
					if (number_of_lines[player] > maximal) {
						maximal = number_of_lines[player];
						for (byte ind = 0; ind < 254; ind++) {
							if (number_of_lines[ind] == maximal) {
								winners[ind] = 1;
							}
							else {
								winners[ind] = 0;
							}
						}
					}
				}
			}

			byte out = 0;
			for (byte player = 0; player < number_of_players; player++) {
				if (winners[player]) {
					if (rand() % (player + 1) == 0) {  // this algorithm lets to chose random player from the chart
						out = player + 1;
					}
				}
			}

			delete number_of_lines;
			return out;
		}
		else {
			ull number_of_lines_to_win = 1;
			for (byte dim = 2; dim < number_of_dimentions; dim++) {
				number_of_lines_to_win *= 3;
			}

			for (byte player = 0; player < number_of_players; player++) {
				if (number_of_lines[player] >= number_of_lines_to_win) {

					delete number_of_lines;
					return player + 1;
				}
			}

			delete number_of_lines;
			return 0;
		}
	}
};



void Draw_warning(const char* text) {
	const byte rectanglecolor[3] = { 255, 64, 64 };
	const byte textcolor[3] = { 255, 255, 255 };
	const byte textfont = 48;

	img.draw_rectangle(
		XSize / 4, YSize / 4,
		XSize * 3 / 4, YSize * 3 / 4,
		rectanglecolor,
		1
	);

	img.draw_text(
		XSize / 3, YSize / 2 - textfont / 2,
		text,
		textcolor, rectanglecolor,
		1, textfont
	);

	img.display(display);
}
bool Checkgamestart() {
	if (number_of_dimentions < 2) {
		Draw_warning("Minimal number of dimentions - 2.");

		return 0;
	}
	if (number_of_dimentions > 6) {
		Draw_warning("Number of dimentions is limited to 6 by local computational power.");

		return 0;
	}
	if (number_of_players < 1) {
		Draw_warning("Minimal number of players - 1.");

		return 0;
	}
	if (number_of_players > 254) {
		Draw_warning("Chosing more than 254 players is not allowed");

		return 0;
	}

	return 1;
}
void Drawstartmenu(bool create_new_inputboxes);
void Gameover(byte winner) {
	img.fill(255);

	if (winner == 0) {
		img.draw_text(100, YSize / 2 - 48, "Draw", black, white, 1, 96);
		img.display(display);

		while (!LMBISPressed) {
			Sleep(10);
		}
	}
	else {
		char text[20] = "Winner: player xxx.";
		ush multiplier = 1;
		for (byte character = 0; character < 3; character++) {
			text[17 - character] = '0' + ((winner / multiplier) % 10);
			multiplier *= 10;
		}

		img.fill(0);
		if (AnimateVictoryScreen) {
			for (uint seed = rand(); 1; seed++) {
				img.draw_text(100, YSize / 2 - 48, text, white, black, 1, 96);

				for (ush xpix = 0; xpix < XSize; xpix++) {
					for (ush ypix = 0; ypix < YSize; ypix++) {
						if (img(xpix, ypix, 0, 0) != 255) {

							img(xpix, ypix, 0, 0) = 127 * (1 + sin(1.0 * (xpix + ypix * (seed & 255) / 128.0) / (120 + seed / 100) + pi * 1 / 120 * seed));
							img(xpix, ypix, 0, 1) = 127 * (1 + sin(1.0 * (xpix + ypix * (seed & 255) / 324.0) / (140 + seed / 30) + pi * 1 / 360 * seed));
							img(xpix, ypix, 0, 2) = 117 * (1 + sin(1.0 * (xpix + ypix * (seed & 255) / 448.0) / (160 + seed / 10) + pi * 1 / 1080 * seed));
						}

						if (LMBISPressed) {
							return;
						}
					}
				}

				img.display(display);
				Sleep(VictoryScreenAnimationDelay);
			}
		}
		else {
			Sleep(50);

			img.draw_text(100, YSize / 2 - 48, text, white, black, 1, 96);

			uint seed = rand();
			for (ush xpix = 0; xpix < XSize; xpix++) {
				for (ush ypix = 0; ypix < YSize; ypix++) {
					if (img(xpix, ypix, 0, 0) != 255) {

						img(xpix, ypix, 0, 0) = 127 * (1 + sin(1.0 * (xpix + ypix * (seed & 255) / 32.0) / (120 + seed * 0.03) + pi * 1 / 60 * seed));
						img(xpix, ypix, 0, 1) = 127 * (1 + sin(1.0 * (xpix + ypix * (seed & 255) / 81.0) / (140 + seed * 0.06) + pi * 1 / 180 * seed));
						img(xpix, ypix, 0, 2) = 117 * (1 + sin(1.0 * (xpix + ypix * (seed & 255) / 112.0) / (160 + seed * 0.18) + pi * 1 / 540 * seed));
					}
				}
			}
			img.display(display);

			while (1) {
				if (LMBISPressed) {
					return;
				}
				Sleep(10);
			}
		}
	}

	img.display(display);
}
void Play() {
	BackToMainMenu = 0;
	if (!Checkgamestart()) {
		Sleep(1000);
		Drawstartmenu(0);

		return;
	}
	for (byte inpbox = 0; inpbox < 2; inpbox++) {
		startmenu_inputboxes[inpbox]->display_this_inputbox = 0;
	}

	Sleep(50);
	img.fill(255);
	Sleep(10);

	Field field(number_of_dimentions);

	byte current_player = 0;
	byte winner = 0;
	bool players_are_playing = 1;
	char text[4];
	while (players_are_playing & (!BackToMainMenu)) {
		text[3] = '\0';
		ush divider = 1;
		for (byte character = 0; character < 3; character++) {
			text[2 - character] = (((current_player + 1) / divider) % 10) + '0';
			divider *= 10;
		}
		img.draw_text(
			XSize - 560, YSize - 240,
			text,
			black, white,
			1, 96
		);
		img.display(display);

		while (!LMBISPressed) {
			Sleep(10);
		}

		if (field.sendclick(display.mouse_x(), display.mouse_y(), current_player)) {
			current_player++;
			current_player = current_player % number_of_players;
		}

		field.draw_tiles();

		winner = field.findwinner();
		if ((winner != 0) | field.check_for_fullness()) {
			players_are_playing = 0;
		}

		Sleep(40);
	}

	Sleep(10);
	Gameover(winner);
	Sleep(10);

	for (byte inpbox = 0; inpbox < 2; inpbox++) {
		startmenu_inputboxes[inpbox]->display_this_inputbox = 1;
	}
	Sleep(50);
	field.set_inputbox_displaying(0);
	Sleep(50);
	BackToMainMenu = 0;

	field.cleanmemory();
}

void Drawhelp() {
	img.draw_text(540, 60, "Extended Tic - tac toe", black, white, 1, 96);

	img.draw_text(480, 220, "What is it", black, white, 1, 36);
	img.draw_text(520, 260, "Extended TTT is almost the same as the classic one, but it also can be played in multiple dimentions and by multiple players.", black, white, 1, 18);

	img.draw_text(480, 300, "Why is it better than the classic one", black, white, 1, 36);
	img.draw_text(520, 340, "ETTT is better, because classic tic tac toe is pretty obvious and trivial, whereas this extended version is really", black, white, 1, 18);
	img.draw_text(520, 360, "tricky and interesting, and, what`s more, almost impossible to fully master, unlike the old one, which you can master in an hour.", black, white, 1, 18);
	img.draw_text(520, 380, "Also it is good for your spacial thinking. It`s a very funny and relatively easy way to learn to imagine higher dimentions, up to 6!", black, white, 1, 18);
	img.draw_text(520, 400, "One more reason why this version is better than the old one is that is`s a huge fun to play with friends (up to 254), because of", black, white, 1, 18);
	img.draw_text(520, 420, "many ridiculous situations happening everytime.", black, white, 1, 18);

	img.draw_text(80, 480, "How to play", black, white, 1, 72);
	img.draw_text(40, 560, "0. Beginning. Chose number of dimentions and number of players for your game. The number of dimentions determines how many coordinates will each point have. In other words, the more dimentions", black, white, 1, 18);
	img.draw_text(40, 580, "you have,the more connections between tiles you have, and the game becomes more complicated. Number of players just determines, obviously, number of players playing in the current game.", black, white, 1, 18);
	img.draw_text(40, 600, "1. Game. Begin the game itself. To claim a tile, just tap on it. In order to win, you need to construct several (3 in power (dimentions - 2)) of tiles (thee tiles staying in a row). If the row is", black, white, 1, 18);
	img.draw_text(40, 620, "interrupted by any other player, it is not a complete line. Pretty much the same as the classic version, except one very important thing. You can construct rows in higher", black, white, 1, 18);
	img.draw_text(40, 640, "dimentions. In order to create a higher-dimentional row, which is actually the same as 2-dimentional row, but fancy-called, you still need to claim the tiles that stay in", black, white, 1, 18);
	img.draw_text(40, 660, "a straight line. But how to find a line if it`s so difficult (but possible) to imagine? There are some ways. The first one is theoretical, but it`s uncomfortable to use.", black, white, 1, 18);
	img.draw_text(40, 680, "And there is also another one, which i personally prefer because it`s very intuitive and gives better understanding of the situation, and it`s simply trying and failing and trying again. ", black, white, 1, 18);
	img.draw_text(40, 700, "Just experimentate and sometime you will find the correlation between your actions and the result. Ans the third one, you can just try to imagine several auqears or cubes laying on each", black, white, 1, 18);
	img.draw_text(40, 720, "other, and these will be your tiles. It works pretty good for the beginning, with 3 or 4 dimentions, but it`s incompetible with any higher dimentions.", black, white, 1, 18);
	img.draw_text(40, 740, "2. Conclusion. If the field is so crowded that no tile is empty anymore, the game result is draw. If someone wins, he, unsurprisingly, wins.", black, white, 1, 18);
	img.draw_text(40, 760, "3. Restart. After the game has ended, you will be transmitted first to the victory/draw screen, and then back to the main menu.", black, white, 1, 18);
}
void Drawstarter(bool create_new_inputboxes) {
	const byte bgcolor[3] = { 192, 192, 206 };
	const byte startbuttoncolor[3] = { 86, 224, 86 };

	img.draw_rectangle(
		0, 0,
		XSize, YSize,
		white, 1
	);

	img.draw_rectangle(
		0, 0,
		352, 442,
		bgcolor, 1
	);
	img.draw_line(352, 0, 352, 442, black, 1);
	img.draw_line(0, 442, 352, 442, black, 1);
	img.draw_line(0, 0, 0, 442, black, 1);
	img.draw_line(0, 0, 352, 0, black, 1);

	if (create_new_inputboxes) {
		startmenu_inputboxes[0] = new inputbox(50, 50, &number_of_dimentions, 2, 6, 0);
		startmenu_inputboxes[1] = new inputbox(200, 50, &number_of_players, 1, 254, 0);
	}
	else {
		startmenu_inputboxes[0]->draw();
		startmenu_inputboxes[1]->draw();
	}
	img.draw_text(30, 250, "Dimentions", black, bgcolor, 1, 24);
	img.draw_text(196, 250, "Players", black, bgcolor, 1, 24);

	img.draw_rectangle(
		30, 300,
		284, 386,
		startbuttoncolor, 1
	);
	img.draw_text(60, 312, "Start", white, startbuttoncolor, 1, 64);
}
void Drawstartmenu(bool create_new_inputboxes) {
	Drawstarter(create_new_inputboxes);
	Drawhelp();

	img.display(display);
}
void Startmenu(bool create_new_inputboxes) {
	Drawstartmenu(create_new_inputboxes);

	while (ProgramIsRunning & (!display.is_closed())) {
		while (!LMBISPressed) {
			Sleep(10);
		}

		if (
			((display.mouse_x() >= 30) & (display.mouse_x() <= 284))
			&
			((display.mouse_y() >= 300) & (display.mouse_y() <= 386))
			)
		{
			Play();
			Drawstartmenu(0);
		}

		Sleep(40);
	}
}



void* track_lmb(void* unneeded_arg) {
	while (ProgramIsRunning) {
		LMBISPressed = ((GetKeyState(VK_LBUTTON) & 0x100) != 0);
		if (LMBISPressed) {
			Sleep(20);
		}
		Sleep(1);
	}

	return 0;
}
int WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       cmdShow)
{
	srand(time(0));

	pthread_t mouse_tracking;
	pthread_create(&mouse_tracking, 0, &track_lmb, 0);

	Startmenu(1);

	while (ProgramIsRunning & (!display.is_closed())) {
		Sleep(100);
	}
	ProgramIsRunning = 0;
	exit(0);
	return 0;
}