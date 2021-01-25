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

	WindowMaker(){}

	void display(vector<double> beatDifferences, AudioStream &stream) //display samples
	{

		sf::RenderWindow window(sf::VideoMode(400, 400), "SFML works!");

		vector<sf::RectangleShape> rectangles(getRectanglesFromData(beatDifferences));

		
		/*
		for (int i = 0; i < 40; count++)
		{
			sf::RectangleShape rectangle(sf::Vector2f(1.f, float(i * 10)));
			rectangle.setFillColor(sf::Color::Green);
			rectangle.setPosition(float(i * 10), float(300 - i * 10));
			rectangles.push_back(rectangle);
		}
		*/
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

			window.clear(sf::Color::Black);

			//draw each rectangle on the window
			for (int i = 0; i < rectangles.size(); i++)
			{
				window.draw(rectangles[i]);
			}

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

		// load the rectangles vector
		double maxRectangleHeight = 300;
		for (int i = 0; i < beatDifferences.size(); i++)
		{
			// TODO: change this to make red/yellow/green rectangles of the appropriate size
			double currentRectangleHeight = beatDifferences[i] * maxRectangleHeight;
			sf::RectangleShape rectangle(sf::Vector2f(1.f, float(currentRectangleHeight)));
			rectangle.setFillColor(sf::Color::Green);
			rectangle.setPosition(float(i * 10), float(maxRectangleHeight - currentRectangleHeight)); //the rectangle starts from the maxRectangleHeight and goes down
			rectangles.push_back(rectangle);
		}

		return rectangles;
	}

};

