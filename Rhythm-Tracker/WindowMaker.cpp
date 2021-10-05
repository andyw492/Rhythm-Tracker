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
#include "AudioAnalyzer.cpp"
#include "RhythmAccuracy.cpp"
#include "AudioStream.cpp"
#include "FileManager.cpp"

using namespace std;

class Button
{
public:

	Button() {}

	Button(float posX, float posY, string s, sf::Font* font)
	{
		int sizeX = 20 + 10 * s.length();
		int sizeY = 50;

		rectangle = sf::RectangleShape(sf::Vector2f(sizeX, sizeY));
		rectangle.setPosition(sf::Vector2f(posX, posY));
		rectangle.setOutlineColor(sf::Color::Black);
		rectangle.setOutlineThickness(-1.f);

		text.setFont(*font);
		text.setString(s);

		// position X formula: (posX + (sizeX / n)), where n = s.length() - 1
		// (placeholder formula for centering text on button)
		text.setPosition(posX + 10, posY + 10);
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

private:

	bool active;
	sf::RectangleShape rectangle;
	sf::Text text;
	sf::Font font;
};

class WindowMaker
{
public:

	WindowMaker() {}

	int checkCount = 0;
	void check()
	{
		cout << "check " << checkCount << endl;
		checkCount++;
	}

	void createWindow()
	{
		display();
	}

	void display()
	{
		sf::RenderWindow window(sf::VideoMode(1600, 900), "Rhythm Tracker");

		// rectangles to be drawn
		vector<sf::RectangleShape> rectangles;
		
		// text font
		sf::Font font;
		if (!font.loadFromFile("fontfile.ttf"))
		{
			cout << "error loading font" << endl;
		}

		// initialize buttons

		// all screens
		Button homeButton = Button(50, 50, "Home", &font);
		Button settingsButton = Button(1470, 50, "Settings", &font);
		Button exitButton = Button(1500, 800, "Exit", &font);
		Button dummyButton; // to ensure indexes of home, settings, and exit remain constant

		for (int i = home; i != enumEnd; i++)
		{
			Screen screen = static_cast<Screen>(i);

			if (screen != home) { comprehensiveButtons[screen].push_back(homeButton); }
			else{ comprehensiveButtons[screen].push_back(dummyButton); }

			if (screen != settings) { comprehensiveButtons[screen].push_back(settingsButton); }
			else { comprehensiveButtons[screen].push_back(dummyButton); }

			comprehensiveButtons[screen].push_back(exitButton);
		}

		// home screen
		comprehensiveButtons[home].push_back(Button(600, 500, "New Recording", &font));
		comprehensiveButtons[home].push_back(Button(800, 500, "Open Recording", &font));

		// settings screen


		// file screen
		comprehensiveButtons[file].push_back(Button(500, 500, "File 1", &font));

		// info screen
		comprehensiveButtons[info].push_back(Button(500, 500, "Start Recording", &font));

		// recording screen
		comprehensiveButtons[recording].push_back(Button(500, 500, "End Recording", &font));

		// analysis screen
		comprehensiveButtons[analysis].push_back(Button(1000, 800, "Restart", &font));
		comprehensiveButtons[analysis].push_back(Button(800, 800, "Save Recording", &font));

		// saveFile screen
		//buttons.push_back(Button(1000, 800, "Restart", &font));
		//buttons.push_back(Button(1200, 800, "Exit", &font));

		// initialize the clock (restart after end of start screen)


		// initialize the audio stream which plays the audio
		

		

		
		string enteredText = "";

		//string fileName = "Recording (228).wav";
		
		int bpm = 120;
		string shortestNoteString = "eighth";

		// file
		int chosenFileIndex = -1;
		string currentFileName = "";

		// recording
		sf::SoundBufferRecorder recorder;
		bool recordingStarted = false;
		bool endRecording = false;

		// analysis
		bool preparedAnalysis = false;
		bool startedClock = false;

		AudioStream stream;
		sf::Clock clock;
		// store time for when going from analysis page to save file page (b/c clock has to be restarted)
		sf::Time storedTime = sf::seconds(0);

		vector<double> noteLocations_seconds;
		int noteLocationCount = 0;
		
		currentScreen = home;

		//--------------------MAIN WINDOW LOOP--------------------------

		while (window.isOpen())
		{
			//-------------------POLL EVENTS----------------------------
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
					// find out which button was clicked on (if any)
					sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
					int buttonIndex = processMouseClick(mousePosition);

					if (buttonIndex == -1) { continue; }

					// carry out button behavior

					// home button
					if (currentScreen != home && buttonIndex == 0) { currentScreen = home; }

					// settings button
					if (currentScreen != settings && buttonIndex == 1) { currentScreen = settings; }
					
					// exit button
					if (buttonIndex == 2) { window.close(); }

					// other buttons
					switch (currentScreen)
					{
						case home:
						{
							if (buttonIndex == 3) { currentScreen = info; }
							if (buttonIndex == 4) { currentScreen = file; }

							break;
						}

						case settings:
						{
							break;
						}

						case file:
						{
							if (buttonIndex > 2) { chosenFileIndex = buttonIndex - 3; }

							break;
						}

						case info:
						{
							if (buttonIndex == 3) { currentScreen = recording; }

							break;
						}

						case recording:
						{
							if (buttonIndex == 3) { endRecording = true; }

							break;
						}

						case analysis:
						{
							// restart button
							if (buttonIndex == 3)
							{
								stream.stop();
								noteLocationCount = 0;
								startedClock = false;
							}

							if (buttonIndex == 4)
							{
								stream.pause();
								startedClock = false;
								storedTime = clock.getElapsedTime();
								currentScreen = saveFile;
							}


							if (buttonIndex == 5) { currentScreen = info; }
							if (buttonIndex == 6) { currentScreen = file; }

							break;
						}

						case saveFile:
						{
							if (buttonIndex == 3) { currentScreen = analysis; }

							break;
						}

						default: break;
					}

					// restart
					//if (buttonIndex == 0)
					//{

					//}
					
				}

			}

			//-------------------DISPLAY SCREENS--------------------------

			switch (currentScreen)
			{
				case home:
				{
					vector<Button> currentScreenButtons = comprehensiveButtons[currentScreen];

					if (enteredText.find("q") != string::npos)
					{
						currentScreen = recording;
					}

					sf::Text text;
					text.setFont(font);
					text.setFillColor(sf::Color::Black);
					text.setPosition(sf::Vector2f(730, 300));
					text.setString("Rhythm Tracker");

					sf::Text text2(text);
					text2.setPosition(sf::Vector2f(770, 450));
					text2.setString("Press Q");

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(text);
					window.draw(text2);

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i].getRectangle());

						sf::Text text(currentScreenButtons[i].getText());
						window.draw(currentScreenButtons[i].getText());
					}

					window.display();
					break;
				}

				case settings:
				{
					vector<Button> currentScreenButtons = comprehensiveButtons[currentScreen];

					sf::Text text;
					text.setFont(font);
					text.setFillColor(sf::Color::Black);
					text.setPosition(sf::Vector2f(730, 300));
					text.setString("Settings Screen");

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(text);

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i].getRectangle());

						sf::Text text(currentScreenButtons[i].getText());
						window.draw(currentScreenButtons[i].getText());
					}

					window.display();

					break;
				}

				case file:
				{
					vector<Button> currentScreenButtons = comprehensiveButtons[currentScreen];

					sf::Text text;
					text.setFont(font);
					text.setFillColor(sf::Color::Black);
					text.setPosition(sf::Vector2f(730, 300));
					text.setString("File Screen");

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(text);

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i].getRectangle());

						sf::Text text(currentScreenButtons[i].getText());
						window.draw(currentScreenButtons[i].getText());
					}

					window.display();

					//--------------------GET AUDIO FROM FILE----------------------

					if (chosenFileIndex == -1) { break; }

					int bpm = -1;
					if (chosenFileIndex == 0)
					{
						currentFileName = "Recording (228).wav";
						bpm = 120;
					}

					loadAudioInfo(currentFileName, bpm, "sixteenth");

					startedClock = false;
					preparedAnalysis = false;
					currentScreen = analysis;

					break;
				}

				case info:
				{
					vector<Button> currentScreenButtons = comprehensiveButtons[currentScreen];

					sf::Text text;
					text.setFont(font);
					text.setFillColor(sf::Color::Black);
					text.setPosition(sf::Vector2f(730, 300));
					text.setString("Info Screen");

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(text);

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i].getRectangle());

						sf::Text text(currentScreenButtons[i].getText());
						window.draw(currentScreenButtons[i].getText());
					}

					window.display();

					break;
				}

				case recording:
				{
					vector<Button> currentScreenButtons = comprehensiveButtons[currentScreen];

					sf::Text text;
					text.setFont(font);
					text.setFillColor(sf::Color::Black);
					text.setPosition(sf::Vector2f(750, 300));
					text.setString("Recording...");

					sf::Text text2(text);
					text2.setPosition(sf::Vector2f(640, 450));
					text2.setString("(Press any key to stop recording)");

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(text);
					window.draw(text2);

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i].getRectangle());

						sf::Text text(currentScreenButtons[i].getText());
						window.draw(currentScreenButtons[i].getText());
					}

					window.display();

					//--------------------RECORD AUDIO--------------------------

					if (!recordingStarted)
					{
						// move beginning of this to start screen

						std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();

						for (int i = 0; i < availableDevices.size(); i++)
						{
							cout << "available devices: " << availableDevices[i] << endl;
						}

						cout << "which device?" << endl;
						int deviceNum = 0; // cin >> deviceNum;

						// choose a device
						std::string inputDevice = availableDevices[deviceNum];

						// set the device
						if (!recorder.setDevice(inputDevice))
						{
							cout << "couldn't set device" << endl;
						}

						// start the capture
						recorder.start();

						newText = false;
						recordingStarted = true;
					}

					if (!endRecording) { break; }

					// stop the capture once text is entered in
					recorder.stop();

					//----------------LOAD INFO FROM AUDIO----------------------

					// retrieve the buffer that contains the captured audio data
					const sf::SoundBuffer& receivedBuffer = recorder.getBuffer();
					buffer = sf::SoundBuffer(receivedBuffer);

					loadAudioInfo("", bpm, shortestNoteString);

					startedClock = false;
					preparedAnalysis = false;
					currentScreen = analysis;

					break;
				}

				case analysis:
				{
					vector<Button> currentScreenButtons = comprehensiveButtons[currentScreen];

					if (!preparedAnalysis)
					{
						// analyze data from given audio
						loadAudioAnalysis();

						rectangles = getRectanglesFromData();

						stream.load(audioInfo.getBuffer());

						// skip all of the audio before the first note in the audio stream
						sf::Time secondsBeforeFirstNote = sf::seconds(audioInfo.getSamplesBeforeFirstNote() / float(audioInfo.getSampleRate() * audioInfo.getChannelCount()));
						stream.setPlayingOffset(secondsBeforeFirstNote);

						// vector of the note locations converted to seconds
						// used to display each rectangle whenever its note is played
						vector<int> noteLocations = audioInfo.getNoteLocations();
						for (int i = 0; i < noteLocations.size(); i++)
						{
							double currentNoteLocation_seconds = (noteLocations[i] / double(audioInfo.getSampleRate() * audioInfo.getChannelCount()));

							// shift the real note locations left so that the audio before the first note can be skipped
							// (the note locations need to be modified because they are used to determine when to display each rectangle)
							currentNoteLocation_seconds -= double(secondsBeforeFirstNote.asSeconds());
							noteLocations_seconds.push_back(currentNoteLocation_seconds);
						}

						preparedAnalysis = true;
					}


					// start the clock and begin playing audio
					if (!startedClock)
					{
						clock.restart();
						stream.play();
						startedClock = true;
					}

					// if it is time to draw the next rectangle (i.e. the elapsed time >= the next note location), 
					// then draw the next rectangle
					if (noteLocationCount < noteLocations_seconds.size() &&
						clock.getElapsedTime().asSeconds() >= noteLocations_seconds[noteLocationCount])
					{
						cout << "drawing rectangle number " << noteLocationCount << endl;

						noteLocationCount++;
					}

					// create the axes
					sf::RectangleShape yAxis(sf::Vector2f(5.f, 500.f));
					yAxis.setFillColor(sf::Color::Black);
					yAxis.setPosition(195.f, 200.f);

					sf::RectangleShape xAxis(sf::Vector2f(800.f, 5.f));
					xAxis.setFillColor(sf::Color::Black);
					xAxis.setPosition(200.f, 447.f);

					// let it play until it is finished
					sf::Time audioDuration = stream.getBuffer().getDuration();
					if (clock.getElapsedTime() + storedTime == audioDuration)
					{
						stream.stop();

						// reset stored time
						storedTime = sf::seconds(0);
					}

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(yAxis);
					window.draw(xAxis);

					for (int i = 0; i < noteLocationCount; i++)
					{
						window.draw(rectangles[i]);
					}

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i].getRectangle());

						sf::Text text(currentScreenButtons[i].getText());
						window.draw(currentScreenButtons[i].getText());
					}

					window.display();

					break;
				}

				case saveFile:
				{
					vector<Button> currentScreenButtons = comprehensiveButtons[currentScreen];

					sf::Text text;
					text.setFont(font);
					text.setFillColor(sf::Color::Black);
					text.setPosition(sf::Vector2f(730, 300));
					text.setString("Save File Screen");

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(text);

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i].getRectangle());

						sf::Text text(currentScreenButtons[i].getText());
						window.draw(currentScreenButtons[i].getText());
					}

					window.display();

					break;
				}
			}

		}

		// save recording of audio
		//fileManager.saveAudioToFile(stream.getBuffer());
		
	}

private:

	string processKeyPress()
	{

	}

	void loadAudioInfo(string fileName, int bpm, string shortestNoteString)
	{

		bool record = (fileName.length() == 0);

		audioInfo.setBpm(bpm);
		audioInfo.setShortestNote(0.5);
		//if (shortestNoteString == "quarter") { audioInfo.setShortestNote(1.0); }
		//if (shortestNoteString == "eighth") { audioInfo.setShortestNote(0.5); }
		//if (shortestNoteString == "sixteenth") { audioInfo.setShortestNote(0.25); }

		AudioAnalyzer analyzer;
		if (record)
		{
			// audioInfo passed by reference
			analyzer.setAudioInfo(audioInfo, buffer);
		}
		else
		{
			// audioInfo passed by reference
			analyzer.setAudioInfo(audioInfo, fileName);
		}
	}

	void loadAudioAnalysis()
	{
		rhythmAccuracy = RhythmAccuracy(audioInfo);

		// debugging
		vector<double> beatDifferences = rhythmAccuracy.getBeatDifferences();
		for (int i = 0; i < beatDifferences.size(); i++)
		{
			if (beatDifferences[i] == INT_MAX)
			{
				cout << "there was no note at beat " << i << endl;
			}
			else
			{
				// beat differences converted from seconds to beats
				cout << "beat " << i << " is " << beatDifferences[i] << " beats off" << endl;
			}
		}
	}

	// check if the mouse click happened on any active buttons
	// returns the index of the button pressed, -1 if no button
	int processMouseClick(sf::Vector2i mousePosition)
	{
		for (int i = 0; i < comprehensiveButtons[currentScreen].size(); i++)
		{
			sf::RectangleShape rectangle = comprehensiveButtons[currentScreen][i].getRectangle();

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

	vector<sf::RectangleShape> getRectanglesFromData()
	{
		vector<sf::RectangleShape> rectangles;
		vector<double> beatDifferences = rhythmAccuracy.getBeatDifferences();

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

	FileManager fileManager;
	AudioInfo audioInfo;
	RhythmAccuracy rhythmAccuracy;

	sf::SoundBuffer buffer;

	bool newText = false;

	enum Screen { home, settings, file, info, recording, analysis, saveFile, enumEnd };
	Screen currentScreen;

	map<Screen, vector<Button>> comprehensiveButtons;
};

