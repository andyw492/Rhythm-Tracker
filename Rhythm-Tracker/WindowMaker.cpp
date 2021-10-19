#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
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
#include "RealTimeRecorder.cpp"

using namespace std;

class Button
{
protected:

	bool active;
	sf::RectangleShape rectangle;
	string buttonString;
	sf::Text text;
	sf::Font font;
	bool isTextFieldBool;

public:

	Button() {}

	Button(float posX, float posY, string s, sf::Font* font)
	{
		cout << "button constructor" << endl;

		int sizeX = 20 + 10 * s.length();
		int sizeY = 50;

		rectangle = sf::RectangleShape(sf::Vector2f(sizeX, sizeY));
		rectangle.setPosition(sf::Vector2f(posX, posY));
		rectangle.setOutlineColor(sf::Color::Black);
		rectangle.setOutlineThickness(-1.f);

		buttonString = s;

		text.setFont(*font);
		text.setString(s);

		// position X formula: (posX + (sizeX / n)), where n = s.length() - 1
		// (placeholder formula for centering text on button)
		text.setPosition(posX + 10, posY + 10);
		text.setFillColor(sf::Color::Black);
		text.setCharacterSize(25);
		active = false;

		isTextFieldBool = false;
	}

	sf::RectangleShape getRectangle()
	{
		return rectangle;
	}

	string getString()
	{
		return buttonString;
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

	void select()
	{
		rectangle.setOutlineColor(sf::Color::Green);
	}

	void deselect()
	{
		rectangle.setOutlineColor(sf::Color::Black);
	}

	bool isTextField()
	{
		return isTextFieldBool;
	}

	virtual void setInputString(string s)
	{
		cout << "in button set input string" << endl;
	}

	virtual sf::Text getInputText()
	{
		sf::Text dummyText;
		dummyText.setString("");
		return dummyText;
	}
};

class TextField : public Button
{
private:

	sf::Text inputText;

public:

	TextField()
	{
		cout << "text field default constructor" << endl;
	}

	TextField(float posX, float posY, string s, sf::Font* font)
	{
		cout << "text field constructor" << endl;

		int sizeX = 20 + 10 * s.length();
		int sizeY = 50;

		rectangle = sf::RectangleShape(sf::Vector2f(sizeX, sizeY));
		rectangle.setPosition(sf::Vector2f(posX, posY));
		rectangle.setOutlineColor(sf::Color::Black);
		rectangle.setOutlineThickness(-1.f);

		buttonString = s;

		text.setFont(*font);
		text.setString(s);

		// position X formula: (posX + (sizeX / n)), where n = s.length() - 1
		// (placeholder formula for centering text on button)

		// for text fields, the text goes to the left of the button
		text.setPosition(posX - (10 * s.length()), posY + 10);
		text.setFillColor(sf::Color::Black);
		text.setCharacterSize(25);

		inputText.setFont(*font);
		inputText.setString("");
		inputText.setPosition(posX + 10, posY + 10);
		inputText.setFillColor(sf::Color::Black);
		inputText.setCharacterSize(25);

		active = false;

		isTextFieldBool = true;
	}

	void setInputString(string s)
	{
		cout << "setting input string to " << s << endl;
		inputText.setString(s);
	}

	sf::Text getInputText()
	{
		return inputText;
	}

};

class WindowMaker
{
public:

	WindowMaker() {}

	~WindowMaker()
	{
		for (int i = home; i != ScreenEnd; i++)
		{
			Screen screen = static_cast<Screen>(i);
			for (int j = 0; j < comprehensiveButtons[screen].size(); j++)
			{
				delete comprehensiveButtons[screen][j];
			}
		}
	}

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

		// text font
		sf::Font font;
		if (!font.loadFromFile("fontfile.ttf"))
		{
			cout << "error loading font" << endl;
		}

		// initialize buttons
		{
			// all screens
			for (int i = home; i != ScreenEnd; i++)
			{
				Screen screen = static_cast<Screen>(i);

				if (screen != home) { comprehensiveButtons[screen].push_back(new Button(50, 50, "Home", &font)); }
				else { comprehensiveButtons[screen].push_back(new Button()); }

				if (screen != settings) { comprehensiveButtons[screen].push_back(new Button(1470, 50, "Settings", &font)); }
				else { comprehensiveButtons[screen].push_back(new Button()); }

				comprehensiveButtons[screen].push_back(new Button(1500, 800, "Exit", &font));
			}

			// home screen
			comprehensiveButtons[home].push_back(new Button(600, 500, "New Recording", &font));
			comprehensiveButtons[home].push_back(new Button(800, 500, "Open Recording", &font));

			// settings screen


			// file screen
			comprehensiveButtons[file].push_back(new Button(500, 500, "File 1", &font));

			// info screen
			comprehensiveButtons[info].push_back(new Button(700, 600, "Start Recording", &font));
			comprehensiveButtons[info].push_back(new TextField(500, 500, "Input Device: ", &font));
			comprehensiveButtons[info].push_back(new TextField(900, 500, "BPM: ", &font));

			// recording screen
			comprehensiveButtons[recording].push_back(new Button(500, 500, "End Recording", &font));

			// analysis screen
			comprehensiveButtons[analysis].push_back(new Button(1000, 800, "Restart", &font));
			comprehensiveButtons[analysis].push_back(new Button(800, 800, "Save Recording", &font));

			// saveFile screen
			comprehensiveButtons[saveFile].push_back(new Button(800, 800, "Save File", &font));
		}

		//buttons.push_back(Button(1000, 800, "Restart", &font));
		//buttons.push_back(Button(1200, 800, "Exit", &font));

		// allows for entering text into text fields (represented by buttons)
		// textInputVariable is the target variable
		// textInputString is the input to that variable
		TextInputVariable textInputVariable = noInput;
		string textInputString = "";

		// error text that displays when the user does something invalid (e.g. enters a string into bpm field)
		sf::Text errorText;
		errorText.setFont(font);
		errorText.setString("");
		errorText.setFillColor(sf::Color::Red);
		errorText.setCharacterSize(25);
		errorText.setPosition(sf::Vector2f(100, 100));

		// file
		int chosenFileIndex = -1;

		// recording
		string inputDevice;
		sf::SoundBufferRecorder recorder;
		sf::SoundBufferRecorder recorder2;
		bool recordingStarted = false;
		bool endRecording = false;
		bool printedAvailable = false; // debugging

		// analysis
		bool playbackAnalysis= false;
		RealTimeRecorder realTimeRecorder;
		bool startedRecorder = false;
		bool preparedAnalysis = false;
		bool startedClock = false;
		vector<sf::RectangleShape> rectangles;

		AudioStream stream;
		sf::Clock clock;
		// store time for when going from analysis page to save file page (b/c clock has to be restarted)
		sf::Time storedTime = sf::seconds(0);

		vector<double> noteLocations_seconds;
		int noteLocationCount = 0;

		Screen currentScreen = home;

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
					cout << "unicode: " << event.text.unicode << endl;

					// if enter key, then process any text that was entered into a text field
					if (event.text.unicode == 13)
					{
						if (textInputVariable != noInput)
						{
							cout << "bpm is " << bpm << endl;
							cout << "text input variable is " << textInputVariable << endl;

							string errorTextString = processTextInput(currentScreen, textInputVariable, textInputString, true);
							if (errorTextString.length() == 0)
							{
								textInputVariable = noInput;
								textInputString = "";
								cout << "audio info bpm: " << audioInfo.getBpm() << endl;
							}
							else
							{
								errorText.setString(errorTextString);
							}
						}
					}

					// if backspace key, then remove one character from textInput
					else if (event.text.unicode == 8)
					{
						// if the text is being entered for fileName, then automatically add .ogg at the end
						if (textInputVariable == fileName && textInputString.length() > 4)
						{
							textInputString = textInputString.substr(0, textInputString.length() - 5) + ".ogg";
						}
						else if (textInputString.length() > 0)
						{
							textInputString = textInputString.substr(0, textInputString.length() - 1);
						}

						processTextInput(currentScreen, textInputVariable, textInputString, false);
					}

					// if escape key, then stop entering text
					else if (event.text.unicode == 27)
					{
						textInputString = "";
						processTextInput(currentScreen, textInputVariable, textInputString, false);

						textInputVariable = noInput;

						// deselect all buttons on the screen
						for (int i = 0; i < comprehensiveButtons[currentScreen].size(); i++)
						{
							comprehensiveButtons[currentScreen][i]->deselect();
						}
					}

					// if regular character that is not backspace or escape, then append it to textInput
					else if (event.text.unicode < 128)
					{
						// if the text is being entered for fileName, then automatically add .ogg at the end
						if (textInputVariable == fileName)
						{
							textInputString = textInputString.substr(0, textInputString.length() - 4) + static_cast<char>(event.text.unicode) + ".ogg";
						}
						else
						{
							textInputString += static_cast<char>(event.text.unicode);
							cout << "text input string: " << textInputString << endl;
						}

						processTextInput(currentScreen, textInputVariable, textInputString, false);

					}
				}

				if (event.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					// if mouse is clicked, then process any text that was entered into a text field
					if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
					{
						if (textInputVariable != noInput)
						{
							cout << "bpm is " << bpm << endl;
							cout << "text input variable is " << textInputVariable << endl;

							string errorTextString = processTextInput(currentScreen, textInputVariable, textInputString, true);
							if (errorTextString.length() == 0)
							{
								textInputVariable = noInput;
								textInputString = "";

								// deselect all buttons on the screen
								for (int i = 0; i < comprehensiveButtons[currentScreen].size(); i++)
								{
									comprehensiveButtons[currentScreen][i]->deselect();
								}
							}
							else
							{
								errorText.setString(errorTextString);
							}
						}
					}

					// find out which button was clicked on (if any)
					sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
					int buttonIndex = processMouseClick(mousePosition, currentScreen, errorText);

					// no button clicked
					if (buttonIndex == -1) { continue; }

					// dont process the click of a button if there is currently an error
					// unless the button being clicked is the exit button
					if (errorText.getString().getSize() > 0 && buttonIndex != 2) { continue; }

					// carry out button behavior

					// home button
					// (special behavior if going to home from analysis)
					if (buttonIndex == 0 && currentScreen != home && currentScreen != analysis) { currentScreen = home; }

					// settings button
					if (buttonIndex == 1 && currentScreen != settings) { currentScreen = settings; }

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

						if (buttonIndex == 4)
						{
							comprehensiveButtons[info][4]->select();
							textInputVariable = TextInputVariable::inputDevice;
							textInputString = "";
						}

						if (buttonIndex == 5)
						{
							comprehensiveButtons[info][5]->select();
							textInputVariable = bpm;
							textInputString = "";
						}

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
							// stop the stream and reset the drawn rectangles
							stream.stop();
							storedTime = sf::seconds(0);
							noteLocationCount = 0;

							// start the stream again
							startedClock = false;
						}

						// save file button
						if (buttonIndex == 4)
						{
							// pause the stream without resetting the drawing progress
							stream.pause();

							// save the clock position (used later to determine when to stop the stream in the analysis screen
							storedTime += clock.getElapsedTime();

							// go to save file screen
							currentScreen = saveFile;
						}

						// if home, new recording, or open recording is clicked, then stop the audio stream
						if (buttonIndex == 0)
						{
							stream.stop();
							storedTime = sf::seconds(0);
							startedClock = false;
							preparedAnalysis = false;

							currentScreen = home;
						}
						if (buttonIndex == 5)
						{
							stream.stop();
							storedTime = sf::seconds(0);
							startedClock = false;
							preparedAnalysis = false;

							currentScreen = info;
						}
						if (buttonIndex == 6)
						{
							stream.stop();
							storedTime = sf::seconds(0);
							startedClock = false;
							preparedAnalysis = false;

							currentScreen = file;
						}

						break;
					}

					case saveFile:
					{
						// save the file and go back to analysis
						if (buttonIndex == 3)
						{
							// get custom file name
							string customFileName = "recording 1";

							// save the file using filemanager
							fileManager.saveAudioToFile(stream.getBuffer(), audioInfo, customFileName);

							// set startedClock to false to resume drawing rectangles
							startedClock = false;

							// go to analysis screen
							currentScreen = analysis;

						}

						// button for entering file name
						if (buttonIndex == 4)
						{

						}

						// button for entering bpm
						if (buttonIndex == 5)
						{

						}

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
				vector<Button*> currentScreenButtons = comprehensiveButtons[currentScreen];

				if (textInputString.find("q") != string::npos)
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

				window.draw(errorText);

				window.draw(text);
				window.draw(text2);

				// draw the buttons
				for (int i = 0; i < currentScreenButtons.size(); i++)
				{

					//cout << "current screen buttons size " << currentScreenButtons.size() << endl;
					window.draw(currentScreenButtons[i]->getRectangle());

					sf::Text text(currentScreenButtons[i]->getText());
					window.draw(currentScreenButtons[i]->getText());
				}


				window.display();
				break;
			}
			
			case settings:
			{
				vector<Button*> currentScreenButtons = comprehensiveButtons[currentScreen];

				sf::Text text;
				text.setFont(font);
				text.setFillColor(sf::Color::Black);
				text.setPosition(sf::Vector2f(730, 300));
				text.setString("Settings Screen");

				//--------------------DRAW TO WINDOW--------------------------

				window.clear(sf::Color::White);

				window.draw(errorText);

				window.draw(text);

				// draw the buttons
				for (int i = 0; i < currentScreenButtons.size(); i++)
				{
					window.draw(currentScreenButtons[i]->getRectangle());

					sf::Text text(currentScreenButtons[i]->getText());
					window.draw(currentScreenButtons[i]->getText());
				}

				window.display();

				break;
			}

			case file:
			{
				playbackAnalysis = true;

				vector<Button*> currentScreenButtons = comprehensiveButtons[currentScreen];

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
					window.draw(currentScreenButtons[i]->getRectangle());

					sf::Text text(currentScreenButtons[i]->getText());
					window.draw(currentScreenButtons[i]->getText());
				}

				window.draw(errorText);

				window.display();

				//--------------------GET AUDIO FROM FILE----------------------

				if (chosenFileIndex == -1) { break; }

				int bpm = 120;
				if (chosenFileIndex == 0)
				{
					audioInfo.setFileName("Recording (228).wav");
					bpm = 120;
				}

				loadAudioInfo();
				
				startedClock = false;
				preparedAnalysis = false;
				currentScreen = analysis;

				break;
			}
			
			case info:
			{
				playbackAnalysis = false;

				vector<Button*> currentScreenButtons = comprehensiveButtons[currentScreen];

				// set bpm, input device
				audioInfo.setBpm(120);
				audioInfo.setInputDevice(0);

				std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();

				if (!printedAvailable)
				{
					for (int i = 0; i < availableDevices.size(); i++)
					{
						cout << "available devices: " << availableDevices[i] << endl;
					}

					printedAvailable = true;
				}


				// choose a device
				inputDevice = availableDevices[audioInfo.getInputDevice()];

				if (!realTimeRecorder.setDevice(inputDevice))
				{
					cout << "couldnt set device" << endl;
				}


				sf::Text text;
				text.setFont(font);
				text.setFillColor(sf::Color::Black);
				text.setPosition(sf::Vector2f(730, 300));
				text.setString("Info Screen");

				//--------------------DRAW TO WINDOW--------------------------

				window.clear(sf::Color::White);

				window.draw(errorText);

				window.draw(text);

				// draw the buttons
				for (int i = 0; i < currentScreenButtons.size(); i++)
				{
					window.draw(currentScreenButtons[i]->getRectangle());

					sf::Text text(currentScreenButtons[i]->getText());
					window.draw(currentScreenButtons[i]->getText());

					sf::Text inputText(currentScreenButtons[i]->getInputText());

					if (inputText.getString().getSize() > 0)
					{
						cout << "input text string: " << (string)inputText.getString() << endl;
						//currentScreenButtons[i]->setInputString(inputText.getString());
						window.draw(inputText);
					}
				}

				window.display();

				break;
			}
			
			case recording:
			{
				currentScreen = analysis;
				break;

				vector<Button*> currentScreenButtons = comprehensiveButtons[currentScreen];

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

				window.draw(errorText);

				window.draw(text);
				window.draw(text2);

				// draw the buttons
				for (int i = 0; i < currentScreenButtons.size(); i++)
				{
					window.draw(currentScreenButtons[i]->getRectangle());

					sf::Text text(currentScreenButtons[i]->getText());
					window.draw(currentScreenButtons[i]->getText());
				}

				window.display();

				//--------------------RECORD AUDIO--------------------------



				//----------------LOAD INFO FROM AUDIO----------------------



				startedClock = false;
				preparedAnalysis = false;
				currentScreen = analysis;

				break;
			}

			case analysis:
			{
				vector<Button*> currentScreenButtons = comprehensiveButtons[currentScreen];

				if (!playbackAnalysis)
				{
					// set channel count and sample rate for real time recorder, and start the recorder
					if (audioInfo.getChannelCount() == 0 || audioInfo.getSampleRate() == 0)
					{
						cout << "channel count " << audioInfo.getChannelCount() << endl;
						cout << "sample rate " << audioInfo.getSampleRate() << endl;
						loadRealTimeAudioInfo(inputDevice);
						cout << "loaded real time info" << endl;
						realTimeRecorder.setChannelCount(audioInfo.getChannelCount());
						realTimeRecorder.setSampleRate(audioInfo.getSampleRate());
					}
					else
					{
						if (!startedRecorder)
						{
							realTimeRecorder.start();
							cout << "started recorder" << endl;
							startedRecorder = true;
						}
					}

					// update audioInfo with the latest version of the buffer
					// unless the buffer has not been loaded with samples yet
					sf::SoundBuffer buffer = realTimeRecorder.getCurrentBuffer();
					cout << "sample count from real time recorder buffer " << buffer.getSampleCount() << endl;
					if (buffer.getSampleCount() > 0)
					{
						audioInfo.setBuffer(realTimeRecorder.getCurrentBuffer());
						loadAudioInfo();

						vector<int> noteLocations = audioInfo.getNoteLocations();
						for (int i = 0; i < noteLocations.size(); i++)
						{
							cout << "note location " << i << ": " << noteLocations[i] << endl;
						}

						// analyze data from given audio
						loadAudioAnalysis();
						rectangles = getRectanglesFromData();
					}


					// create the axes
					sf::RectangleShape yAxis(sf::Vector2f(5.f, 500.f));
					yAxis.setFillColor(sf::Color::Black);
					yAxis.setPosition(195.f, 200.f);

					sf::RectangleShape xAxis(sf::Vector2f(800.f, 5.f));
					xAxis.setFillColor(sf::Color::Black);
					xAxis.setPosition(200.f, 447.f);

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(errorText);

					window.draw(yAxis);
					window.draw(xAxis);

					for (int i = 0; i < rectangles.size(); i++)
					{
						window.draw(rectangles[i]);
					}

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i]->getRectangle());

						sf::Text text(currentScreenButtons[i]->getText());
						window.draw(currentScreenButtons[i]->getText());
					}


					window.display();
				}
				// if playbackAnalysis
				else
				{
					if (!preparedAnalysis)
					{
						realTimeRecorder.stop();

						// analyze data from given audio
						loadAudioAnalysis();

						rectangles = getRectanglesFromData();

						stream.load(audioInfo.getBuffer());

						// reset variables from previous analysis session
						noteLocationCount = 0;
						noteLocations_seconds.clear();

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

					sf::Time audioDuration = stream.getBuffer().getDuration();

					// start the clock and begin playing audio
					if (!startedClock)
					{
						if (storedTime < audioDuration)
						{
							clock.restart();
							stream.play();
						}

						startedClock = true;
					}

					// if it is time to draw the next rectangle (i.e. the elapsed time >= the next note location),
					// then draw the next rectangle
					if (noteLocationCount < noteLocations_seconds.size() &&
						(clock.getElapsedTime() + storedTime).asSeconds() >= noteLocations_seconds[noteLocationCount])
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
					if (clock.getElapsedTime() + storedTime >= audioDuration)
					{
						stream.stop();

						// reset stored time
						storedTime = sf::seconds(0);
					}

					//--------------------DRAW TO WINDOW--------------------------

					window.clear(sf::Color::White);

					window.draw(errorText);

					window.draw(yAxis);
					window.draw(xAxis);

					for (int i = 0; i < noteLocationCount; i++)
					{
						// edge case: no notes were found
						if (rectangles.size() <= 0) { continue; }

						window.draw(rectangles[i]);
					}

					// draw the buttons
					for (int i = 0; i < currentScreenButtons.size(); i++)
					{
						window.draw(currentScreenButtons[i]->getRectangle());

						sf::Text text(currentScreenButtons[i]->getText());
						window.draw(currentScreenButtons[i]->getText());
					}


					window.display();
				}

				break;
			}

			case saveFile:
			{
				vector<Button*> currentScreenButtons = comprehensiveButtons[currentScreen];

				sf::Text text;
				text.setFont(font);
				text.setFillColor(sf::Color::Black);
				text.setPosition(sf::Vector2f(730, 300));
				text.setString("Save File Screen");

				//--------------------DRAW TO WINDOW--------------------------

				window.clear(sf::Color::White);

				window.draw(errorText);

				window.draw(text);

				// draw the buttons
				for (int i = 0; i < currentScreenButtons.size(); i++)
				{
					window.draw(currentScreenButtons[i]->getRectangle());

					sf::Text text(currentScreenButtons[i]->getText());
					window.draw(currentScreenButtons[i]->getText());
				}


				window.display();

				break;
			}
			}

		}

		// save recording of audio
		//

	}

private:

	// gets the note locations
	AudioInfo audioInfo;

	// gets the accuracy of each note
	RhythmAccuracy rhythmAccuracy;

	// allows for saving and opening audio files
	FileManager fileManager;

	enum Screen { home, settings, file, info, recording, analysis, saveFile, ScreenEnd };
	enum TextInputVariable { noInput, inputDevice, bpm, fileName, TextInputVariableEnd };

	// stores all clickable buttons for all screens
	map<Screen, vector<Button*>> comprehensiveButtons;

	string processKeyPress()
	{

	}

	void loadAudioInfo()
	{
		string fileName = audioInfo.getFileName();
		int bpm = audioInfo.getBpm();
		sf::SoundBuffer buffer = audioInfo.getBuffer();

		bool record = (fileName.length() == 0);

		audioInfo.setFileName(fileName);
		audioInfo.setBpm(bpm);

		// shortest note is set to eighth note by default (make changable in settings later)
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
	int processMouseClick(sf::Vector2i mousePosition, Screen currentScreen, sf::Text errorText)
	{
		int buttonIndex = -1;

		for (int i = 0; i < comprehensiveButtons[currentScreen].size(); i++)
		{
			sf::RectangleShape rectangle = comprehensiveButtons[currentScreen][i]->getRectangle();

			bool clickedButton =
				(
				(mousePosition.x > rectangle.getPosition().x &&
					mousePosition.x < rectangle.getPosition().x + rectangle.getSize().x) &&
					(mousePosition.y > rectangle.getPosition().y &&
						mousePosition.y < rectangle.getPosition().y + rectangle.getSize().y)
					);

			if (clickedButton) { buttonIndex = i; }
		}

		// if there is currently no error or if the user is trying to exit, return the button index
		// (otherwise, pretend the user didn't press any button)
		if (string(errorText.getString()).length() == 0 || buttonIndex == 2)
		{
			return buttonIndex;
		}

		return -1;
	}

	void playMetronome(int bpm)
	{

	}

	// check if the text input should return any error text
	// if no error text, then set audioInfo variable
	string processTextInput(Screen currentScreen, TextInputVariable targetVariable, string textInput, bool validateInput)
	{
		cout << "process text input: " << endl;
		cout << "target variable: " << (targetVariable == bpm) << endl;
		cout << "text input: " << textInput << endl;

		// store the current text input into its text field
		switch (currentScreen)
		{
		case home: break;
		case settings: break;
		case file: break;
		case info:
		{
			switch (targetVariable)
			{
			case bpm:
			{
				comprehensiveButtons[info][5]->setInputString(textInput);
				cout << "bpm text field string: " << (string)comprehensiveButtons[info][5]->getInputText().getString() << endl;
				break;
			}

			default: break;
			}
		}
		case recording: break;
		case analysis: break;
		case saveFile: break;
		default: break;
		}

		// validate the input if enter key pressed or mouse clicked
		if (validateInput)
		{
			bool isNumber = true;
			for (int i = 0; i < textInput.length(); i++)
			{
				if (!isdigit(textInput[i])) { isNumber = false; }
			}

			switch (targetVariable)
			{
			case bpm:
			{
				if (!isNumber) { return "BPM must be an integer!"; }

				int bpmInput = stoi(textInput);
				if (bpmInput <= 0) { return "Invalid BPM value!"; }

				audioInfo.setBpm(bpmInput);
				return "";
			}
			case fileName:
			{
				if (textInput.length() > getMaxFileNameLength()) { return "File Name Too Long!"; }

				audioInfo.setFileName(textInput);
				return "";
			}
			default:
			{
				cout << "invalid target variable" << endl;
				string s; cin >> s;
				exit(1);
			}
			}
		}

		return "";
	}

	vector<sf::RectangleShape> getRectanglesFromData()
	{
		vector<sf::RectangleShape> rectangles;
		vector<double> beatDifferences = rhythmAccuracy.getBeatDifferences();

		double maxRectangleHeight = getMaxRectangleHeight();

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
			else if (abs(beatDifferences[i]) > yellowThreshold)
			{
				rectangle.setFillColor(sf::Color::Red);
			}

			rectangle.setOutlineThickness(-1.f); // make the outline go inside the rectangle
			rectangles.push_back(rectangle);


		}

		return rectangles;
	}

	// set channel count and sample rate into audio info for real time recording
	void loadRealTimeAudioInfo(string inputDevice)
	{
		sf::SoundBufferRecorder recorder;
		if (!recorder.setDevice(inputDevice))
		{
			cout << "couldn't set input device" << endl;
		}

		bool firstStart = false;

		sf::Clock clock;

		while (true)
		{
			if (!firstStart)
			{
				recorder.start();
				firstStart = true;
			}

			if (clock.getElapsedTime().asMilliseconds() < 50) { continue; }

			recorder.stop();

			const sf::SoundBuffer& receivedBuffertemp = recorder.getBuffer();
			sf::SoundBuffer buffertemp = sf::SoundBuffer(receivedBuffertemp);
			if (buffertemp.getSampleCount() > 0)
			{
				audioInfo.setChannelCount(buffertemp.getChannelCount());
				audioInfo.setSampleRate(buffertemp.getSampleRate());
				break;
			}
			else
			{
				recorder.start();
			}
			
		}

		recorder.stop();

		return;

	}

};