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

	void getDisplaySamples(short* samples, int sampleCount, short* &displaySamples)
	{
		short minSample = SHRT_MAX;
		short maxSample = SHRT_MIN;
		int minSampleCount;
		int maxSampleCount;

		int displaySampleCount = 0;
		for (int i = 0; i < sampleCount; i++)
		{

			if (samples[i] < minSample)
			{
				minSample = samples[i];
				minSampleCount = i;
			}
			if (samples[i] > maxSample)
			{
				maxSample = samples[i];
				maxSampleCount = i;
			}

			if (i % 25000 == 0 && i > 0)
			{
				displaySamples[displaySampleCount] = maxSample;

				minSample = SHRT_MAX;
				maxSample = SHRT_MIN;
				displaySampleCount++;
			}

		}
	}

	void getRectangles(vector<sf::RectangleShape> &rectangles, short* displaySamples, int displaySamplesAmount)
	{
		short maxSample = SHRT_MIN;
		double* displaySampleProportions = new double[displaySamplesAmount];

		cout << "\n\ndisplay samples:" << endl;
		//load displaySampleProportions, which represent displaySamples as proportions of the maximum displaySample
		//e.g. if a displaySamplePropotion == 0.5, then it is half the value of the maximum displaySample
		for (int i = 0; i < displaySamplesAmount; i++)
		{
			if (displaySamples[i] > maxSample)
			{
				maxSample = displaySamples[i];
			}
			cout << "count: " << i << " sample: " << displaySamples[i] << endl;
		}

		for (int i = 0; i < displaySamplesAmount; i++)
		{
			displaySampleProportions[i] = (double(displaySamples[i]) / double(maxSample));
			cout << "proportion: " << displaySampleProportions[i] << endl;
		}

		//load the rectangles vector with displaySampleProportions
		double maxRectangleHeight = 300;
		for (int i = 0; i < displaySamplesAmount; i++)
		{
			double currentRectangleHeight = displaySampleProportions[i] * maxRectangleHeight;
			sf::RectangleShape rectangle(sf::Vector2f(1.f, float(currentRectangleHeight)));
			rectangle.setFillColor(sf::Color::Green);
			rectangle.setPosition(float(i * 10), float(maxRectangleHeight - currentRectangleHeight)); //the rectangle starts from the maxRectangleHeight and goes down
			rectangles.push_back(rectangle);
		}

		delete[] displaySampleProportions;

	}


	void display(short* displaySamples, int displaySamplesAmount, AudioStream &stream) //display samples
	{

		sf::RenderWindow window(sf::VideoMode(400, 400), "SFML works!");

		vector<sf::RectangleShape> rectangles;

		getRectangles(rectangles, displaySamples, displaySamplesAmount);
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

};

