#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cassert>
#include <map>
#include <algorithm>

#include "common.h"
#include "AudioInfo.h"
#include "AudioStream.cpp"

using namespace std;

class Button
{
public:

	Button() {}

	Button(float posX, float posY, string s, sf::Font* font)
	{
		int sizeX = 100;
		int sizeY = 50;

		rectangle = sf::RectangleShape(sf::Vector2f(sizeX, sizeY));
		rectangle.setPosition(sf::Vector2f(posX, posY));
		rectangle.setOutlineColor(sf::Color::Black);
		rectangle.setOutlineThickness(-1.f);

		text.setFont(*font);
		text.setString(s);

		// position X formula: (posX + (sizeX / n)), where n = s.length() - 1
		// (placeholder formula for centering text on button)
		text.setPosition(posX + (sizeX / (s.length() - 1)), posY + (sizeY / 5));
		text.setFillColor(sf::Color::Black);
		text.setCharacterSize(25);
		active = false;
	}

	sf::RectangleShape getRectangle()
	{
		return rectangle;
	}

	sf::Text getText()
	{
		return text;
	}

	const sf::Font* getFont()
	{
		return text.getFont();
	}

	bool isActive()
	{
		return active;
	}

	void setActive(bool active)
	{
		this->active = active;
	}

	//void draw(sf::RenderWindow& window)
	//{
	//	window.draw(rectangle);
	//	window.draw(text);
	//}

private:

	bool active;
	sf::RectangleShape rectangle;
	sf::Text text;
	sf::Font font;
};

class WindowMaker
{
public:

	WindowMaker()
	{
		displayStartScreen = true;
	}

	int checkCount = 0;
	void check()
	{
		cout << "check " << checkCount << endl;
		checkCount++;
	}

	void display(vector<double> beatDifferences, sf::SoundBuffer buffer)
	{
		sf::RenderWindow window(sf::VideoMode(1600, 900), "Rhythm Tracker");

		vector<sf::RectangleShape> rectangles(getRectanglesFromData(beatDifferences));
		
		// text font
		sf::Font font;
		if (!font.loadFromFile("fontfile.ttf"))
		{
			cout << "error loading font" << endl;
		}

		// initialize buttons
		vector<Button> buttons;
		buttons.push_back(Button(1000, 800, "Restart", &font));
		buttons.push_back(Button(1200, 800, "Exit", &font));

		// initialize the clock
		sf::Clock clock;
		bool startedClock = false;

		// initialize our custom stream
		AudioStream stream;
		stream.load(buffer);

		// skip all of the audio before the first note in the audio stream
		sf::Time secondsBeforeFirstNote = sf::seconds(AudioInfo::samplesBeforeFirstNote / float(AudioInfo::sampleRate * AudioInfo::channelCount));
		stream.setPlayingOffset(secondsBeforeFirstNote);
		cout << "playing offset: " << secondsBeforeFirstNote.asSeconds() << endl;
		
		// vector of the note locations converted to seconds
		// used to display each rectangle whenever its note is played
		vector<double> noteLocations_seconds;
		cout << "notelocations size is " << AudioInfo::noteLocations.size() << endl;
		for (int i = 0; i < AudioInfo::noteLocations.size(); i++)
		{
			double currentNoteLocation_seconds = (AudioInfo::noteLocations[i] / double(AudioInfo::sampleRate * AudioInfo::channelCount));

			// shift the real note locations left so that the audio before the first note can be skipped
			// (the note locations need to be modified because they are used to determine when to display each rectangle)
			currentNoteLocation_seconds -= double(secondsBeforeFirstNote.asSeconds());

			noteLocations_seconds.push_back(currentNoteLocation_seconds);
		}

		

		int noteLocationCount = 0;

		string enteredText = "";
		bool newText = false;

		//--------------------MAIN WINDOW LOOP--------------------------

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();

				// for start screen
				if (event.type == sf::Event::TextEntered)
				{
					if (event.text.unicode < 128)
					{
						enteredText += static_cast<char>(event.text.unicode);
						newText = true;
					}
				}

				if (event.type == sf::Event::MouseButtonPressed)
				{
					sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
					int buttonIndex = processMouseClick(mousePosition, buttons);

					// restart
					if (buttonIndex == 0)
					{
						stream.stop();
						noteLocationCount = 0;
						startedClock = false;
					}
					
					// exit
					if (buttonIndex == 1)
					{
						window.close();
					}
				}

			}

			//--------------------START SCREEN--------------------------
			//if (displayStartScreen)
			if(false)
			{
				vector<int> activeButtons = {};

				for (int i = 0; i < buttons.size(); i++)
				{
					if (find(activeButtons.begin(), activeButtons.end(), i) != activeButtons.end())
					{
						buttons[i].setActive(true);
					}
					else
					{
						buttons[i].setActive(false);
					}
				}

				if (enteredText.find("q") != string::npos)
				{
					displayStartScreen = false;
				}

				window.clear(sf::Color::White);

				for (int i = 0; i < buttons.size(); i++)
				{
					//buttons[i].draw(window);
				}

				sf::Text text;
				text.setFont(font);
				text.setFillColor(sf::Color::Black);
				text.setPosition(sf::Vector2f(100, 100));
				text.setString(enteredText);

				window.draw(text);
				



				window.display();				
			}
			//--------------------MAIN SCREEN---------------------------
			else
			{
				vector<int> activeButtons = {0, 1};

				for (int i = 0; i < buttons.size(); i++)
				{
					if (find(activeButtons.begin(), activeButtons.end(), i) != activeButtons.end())
					{
						buttons[i].setActive(true);
					}
					else
					{
						buttons[i].setActive(false);
					}
				}

				window.clear(sf::Color::White);

				// start the clock and begin playing audio
				if (!startedClock)
				{
					clock.restart();
					stream.play();
					startedClock = true;
				}

				//cout << "audio duration " << stream.getBuffer().getDuration().asSeconds() << endl;
				//cout << "notelocations_seconds size " << noteLocations_seconds.size() << endl;
				//cout << "notelocations_seconds 0 " << noteLocations_seconds[0] << endl;
				//cout << "notelocations_seconds 1 " << noteLocations_seconds[1] << endl;

				//cout << "noteLocationCount < noteLocations_seconds.size() " << (noteLocationCount < noteLocations_seconds.size()) << endl;
				//cout << "clock.getElapsedTime().asSeconds() >= noteLocations_seconds[noteLocationCount] " << (clock.getElapsedTime().asSeconds() >= noteLocations_seconds[noteLocationCount]) << endl;

				// if it is time to draw the next rectangle (i.e. the elapsed time >= the next note location), 
				// then draw the next rectangle

				if (noteLocationCount < noteLocations_seconds.size() &&
					clock.getElapsedTime().asSeconds() >= noteLocations_seconds[noteLocationCount])
				{
					cout << "drawing rectangle number " << noteLocationCount << endl;

					noteLocationCount++;
				}

				for (int i = 0; i < noteLocationCount; i++)
				{
					window.draw(rectangles[i]);
				}

				// draw the axes
				sf::RectangleShape yAxis(sf::Vector2f(5.f, 500.f));
				yAxis.setFillColor(sf::Color::Black);
				yAxis.setPosition(195.f, 200.f);

				sf::RectangleShape xAxis(sf::Vector2f(800.f, 5.f));
				xAxis.setFillColor(sf::Color::Black);
				xAxis.setPosition(200.f, 447.f);

				window.draw(yAxis);
				window.draw(xAxis);

				// draw the active buttons
				for (int i = 0; i < buttons.size(); i++)
				{
					window.draw(buttons[i].getRectangle());

					sf::Text text(buttons[i].getText());
					//const sf::Font* font = buttons[i].getFont();
					//sf::Font font2;
					//font2.loadFromFile("arial.ttf");
					//text.setFont(*font);
					window.draw(buttons[i].getText());
				}

				window.display();

				// let it play until it is finished
				sf::Time audioDuration = stream.getBuffer().getDuration();
				if (clock.getElapsedTime() == audioDuration)
				{
					stream.stop();
				}

			}

		}
		
	}

private:

	string processKeyPress()
	{

	}

	// check if the mouse click happened on any active buttons
	// returns the index of the button pressed, -1 if no button
	int processMouseClick(sf::Vector2i mousePosition, vector<Button> buttons)
	{
		for (int i = 0; i < buttons.size(); i++)
		{
			if (!buttons[i].isActive()) { continue; }

			sf::RectangleShape rectangle = buttons[i].getRectangle();

			bool clickedButton = 
			(
				(mousePosition.x > rectangle.getPosition().x && 
					mousePosition.x < rectangle.getPosition().x + rectangle.getSize().x) &&
				(mousePosition.y > rectangle.getPosition().y &&
					mousePosition.y < rectangle.getPosition().y + rectangle.getSize().y)
			);

			if (clickedButton) { return i; }
		}

		return -1;
	}

	void playMetronome(int bpm)
	{

	}

	vector<sf::RectangleShape> getRectanglesFromData(vector<double> beatDifferences)
	{
		vector<sf::RectangleShape> rectangles;

		printf("calling max rectangle height\n");
		double maxRectangleHeight = getMaxRectangleHeight();
		printf("max rectangle height: %d\n", maxRectangleHeight);

		double maxBeatDifference = 0.1;

		// thresholds for coloring a rectangle red, yellow, or green
		// e.g. a rectangle is colored green if it is less than (0.3 * maxBeatDifference)
		double greenThreshold = 0.3 * maxBeatDifference;
		double yellowThreshold = 0.6 * maxBeatDifference;

		for (int i = 0; i < beatDifferences.size(); i++)
		{
			// if beatDifferences[i] is INT_MAX (i.e. there is no note at that beat), then skip forward one i position
			// (the rectangles' x-coordinates are based on i)
			if (beatDifferences[i] == INT_MAX) { continue; }

			// maxBeatDifference should be the largest size for a rectangle to be displayed
			if (abs(beatDifferences[i]) > abs(maxBeatDifference))
			{
				// set beatDifferences[i] to 0.1 while keeping the +/- the same
				beatDifferences[i] *= (maxBeatDifference / abs(beatDifferences[i]));
			}

			// rectangle height uses the proportion of the current beat difference to the max beat difference
			// and applies it to the max rectangle height
			double currentRectangleHeight = (beatDifferences[i] / maxBeatDifference) * maxRectangleHeight;

			sf::RectangleShape rectangle(sf::Vector2f(10.f, float(currentRectangleHeight)));
			rectangle.setPosition(float(200 + i * 15), 450.f); //the rectangle starts from the maxRectangleHeight and goes down
			rectangle.setOutlineColor(sf::Color::Black);

			// set the color of the rectangle to red/yellow/green
			if (abs(beatDifferences[i]) < greenThreshold)
			{
				rectangle.setFillColor(sf::Color::Green);
			}
			else if (abs(beatDifferences[i]) > greenThreshold && abs(beatDifferences[i]) < yellowThreshold)
			{
				rectangle.setFillColor(sf::Color::Yellow);
			}
			else if(abs(beatDifferences[i]) > yellowThreshold)
			{
				rectangle.setFillColor(sf::Color::Red);
			}

			rectangle.setOutlineThickness(-1.f); // make the outline go inside the rectangle
			rectangles.push_back(rectangle);
		}

		return rectangles;
	}


	bool displayStartScreen;

};

