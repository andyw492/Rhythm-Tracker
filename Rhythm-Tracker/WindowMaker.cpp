#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>

#include "AudioStream.cpp"

using namespace std;

class WindowMaker
{
public:

	WindowMaker() {}

	void display(vector<double> beatDifferences, AudioStream &stream)
	{

		sf::RenderWindow window(sf::VideoMode(1600, 900), "Rhythm Tracker");

		vector<sf::RectangleShape> rectangles(getRectanglesFromData(beatDifferences));
		sf::Clock clock;
		//play our custom stream
		stream.play();

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}

			window.clear(sf::Color::White);

			//draw each rectangle on the window
			for (int i = 0; i < rectangles.size(); i++)
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

			sf::Time audioDuration = stream.getBuffer().getDuration();

			// let it play until it is finished
			if (clock.getElapsedTime() == audioDuration)
			{
				stream.stop();
			}


		}
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

};

