#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <cassert>

#include "AudioInfo.h"
#include "AudioStream.cpp"

using namespace std;

class WindowMaker
{
public:

	WindowMaker()
	{
		displayStartScreen = true;
	}

	void display(vector<double> beatDifferences, AudioStream &stream)
	{
		sf::RenderWindow window(sf::VideoMode(1600, 900), "Rhythm Tracker");

		vector<sf::RectangleShape> rectangles(getRectanglesFromData(beatDifferences));
		sf::Clock clock;

		// skip all of the audio before the first note in the audio stream
		sf::Time secondsBeforeFirstNote = sf::seconds(AudioInfo::samplesBeforeFirstNote / float(AudioInfo::sampleRate * AudioInfo::channelCount));
		stream.setPlayingOffset(secondsBeforeFirstNote);
		stream.play();
		
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


		while (window.isOpen())
		{
			//--------------------START SCREEN--------------------------
			if (displayStartScreen)
			{
				cout << "at start screen" << endl;
				displayStartScreen = false;
			}
			//--------------------MAIN SCREEN---------------------------
			else
			{
				
				
				sf::Event event;
				while (window.pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
						window.close();

				}

				window.clear(sf::Color::White);

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

	vector<sf::RectangleShape> getRectanglesFromData(vector<double> beatDifferences)
	{
		vector<sf::RectangleShape> rectangles;

		double maxRectangleHeight = 250;
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

