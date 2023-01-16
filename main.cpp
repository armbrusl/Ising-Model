#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <random>						// std::default_random_engine


using namespace std::this_thread;		 // sleep_for, sleep_until
using namespace std::chrono_literals;	 // ns, us, ms, s, h, etc.
 

 class Ising : public olc::PixelGameEngine
 {
 public:
	 int L = 300;
	 float J = 1.0;
	 float H = 0.0;
	 float temp = 1.0;
	 int S[300][300];
	 std::array<int, 300 * 300> nodes;

	 float energy_difference = 0, r = 0, magnetization = 0, energy = 0, magnitude_variable_change = 0.005, seed = 0;
	 int spin = 0, nspins = 0, length = 0, evals = 0;
	 int* node = (int*)S;
	 std::string result, sText;

 public:


 	bool OnUserCreate() override
    {
        //Called once at the start, so create things here

		length = L * L;
		evals = 0;

		for (int i = 0; i < length; i++)
		{
			nodes[i] = i;
		}

		initialize();


 		return true;
 	}

	void print_variables()
	{
		std::stringstream sText1, sText2, sText3, sText4, sText5;

		sText = std::to_string(evals);
		DrawString(L + 5, 10, sText, olc::WHITE, 1);

		sText1 << ceil(H*100)/100;
		result = sText1.str();
		DrawString(L + 5, 30, "H=", olc::WHITE, 1);
		DrawString(L + 20, 30, result, olc::WHITE, 1);

		sText2 << ceil(J * 100) / 100;
		result = sText2.str();
		DrawString(L + 5, 50, "J=", olc::WHITE, 1);
		DrawString(L + 20, 50, result, olc::WHITE, 1);

		sText3 << ceil(temp * 100) / 100;
		result = sText3.str();
		DrawString(L + 5, 70, "T=", olc::WHITE, 1);
		DrawString(L + 20, 70, result, olc::WHITE, 1);

		sText4 << ceil(magnetization * 100) / 100;
		result = sText4.str();
		DrawString(L + 5, 110, "M=", olc::WHITE, 1);
		DrawString(L + 20, 110, result, olc::WHITE, 1);

		sText5 << ceil(energy * 100) / 100;
		result = sText5.str();
		DrawString(L + 5, 130, "E=", olc::WHITE, 1);
		DrawString(L + 20, 130, result, olc::WHITE, 1);
	}

	void keyboard_input()
	{
		// Complete Reset
		if (GetKey(olc::Key::I).bHeld) 
		{
			initialize();
			evals = 0;
			H = 0.0;
			J = 1.0;
			temp = 1.0;
		}

		// Increasing and decreasing the variables
		if (GetKey(olc::Key::H).bHeld && GetKey(olc::Key::UP).bHeld) 
		{
			H += magnitude_variable_change;
		}
		if (GetKey(olc::Key::H).bHeld && GetKey(olc::Key::DOWN).bHeld) 
		{
			H -= magnitude_variable_change;
		}

		if (GetKey(olc::Key::J).bHeld && GetKey(olc::Key::UP).bHeld)
		{
			J += magnitude_variable_change;
		}
		if (GetKey(olc::Key::J).bHeld && GetKey(olc::Key::DOWN).bHeld) 
		{
			J -= magnitude_variable_change;
		}

		if (GetKey(olc::Key::T).bHeld && GetKey(olc::Key::UP).bHeld) 
		{
			temp += magnitude_variable_change;
		}
		if (GetKey(olc::Key::T).bHeld && GetKey(olc::Key::DOWN).bHeld) 
		{
			temp -= magnitude_variable_change;
		}

		// Resetting to Initial Values individually
		if (GetKey(olc::Key::H).bHeld && GetKey(olc::Key::ENTER).bPressed) 
		{
			H = 0.0;
		}
		if (GetKey(olc::Key::J).bHeld && GetKey(olc::Key::ENTER).bPressed) 
		{
			J = 0.0;
		}
		if (GetKey(olc::Key::T).bHeld && GetKey(olc::Key::ENTER).bPressed)
		{
			temp = 1.0;
		}


		print_variables();
	}

	void mouse_input()
	{
		if (GetMouse(0).bHeld)
		{
			int mouse_pos[2] = {int(GetMouseX()), int(GetMouseY())};
			int color = -1 * S[mouse_pos[0]][mouse_pos[1]];

			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					int color = -1 * S[mouse_pos[0] + i][mouse_pos[1] + j];
					Draw(mouse_pos[0] + i, mouse_pos[1] + j, olc::Pixel(color * 255, color * 255, color * 255));

					S[mouse_pos[0] + i][mouse_pos[1] + j] = 1;


				}


			}

			Draw(mouse_pos[0], mouse_pos[1], olc::Pixel(color * 255, color * 255, color * 255));

			S[mouse_pos[0]][mouse_pos[1]] = -1 * S[mouse_pos[0]][mouse_pos[1]];

		}

		if (GetMouse(1).bHeld)
		{
			int mouse_pos[2] = { int(GetMouseX()), int(GetMouseY()) };
			int color = -1 * S[mouse_pos[0]][mouse_pos[1]];

			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					int color = -1 * S[mouse_pos[0] + i][mouse_pos[1] + j];
					Draw(mouse_pos[0] + i, mouse_pos[1] + j, olc::Pixel(color * 255, color * 255, color * 255));

					S[mouse_pos[0] + i][mouse_pos[1] + j] = -1;


				}


			}

			Draw(mouse_pos[0], mouse_pos[1], olc::Pixel(color * 255, color * 255, color * 255));

			S[mouse_pos[0]][mouse_pos[1]] = -1 * S[mouse_pos[0]][mouse_pos[1]];

		}



	}
 
 	bool OnUserUpdate(float fElapsedTime) override
    {
		// Gets Called once a Frame
 		Clear(olc::BLACK); //Clear's the past screens.
		move();
		draw();
		keyboard_input();

		mouse_input();
		sleep_for(1ms);
	
 		return true;
 	}
 
	void initialize() 
	{
		for (int i : nodes)
		{
			(rand() % 2 == 0) ? *(node + i) = 1 : *(node + i) = -1;
		}
	}

	void draw()
	{
		for (int i = 0; i < L; i++)
		{
			for (int j = 0; j < L; j++)
			{
				(S[i][j] == 1)? Draw(i, j, olc::WHITE) : 0;
			}
		}
	}

	int neighboorspins(int ind) 
	{
		// The four neighboors
		return (*(node + (ind + 1) % length) + *(node + (ind + length - 1) % length) + *(node + (ind + L) % length)+ *(node + (ind + length - L) % length));
	}

	void move() 
	{
		seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(nodes.begin(), nodes.end(), std::default_random_engine(seed));

		magnetization = 0.0;
		energy = 0.0;

		for (int i : nodes)
		{
			spin = *(node + i);
			nspins = neighboorspins(i);

			energy_difference = 2 * (J * spin * nspins + H * spin);

			r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			if (energy_difference < 0 || r < exp(-(1 / temp) * energy_difference))
			{
				*(node + i) = -1*spin;
			}
		
			magnetization += 1 * *(node + i);
			energy += (-J * spin * nspins + H * spin);
		}
		evals += 1;

		magnetization = abs(magnetization) / length;
		energy = (energy/2.0) / length;
	}
 };
 
 int main()
 {
 	Ising app;

 	if (app.Construct(450, 300, 2, 2))
 		app.Start();
 
 	return 0;
 }



