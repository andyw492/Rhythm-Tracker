#include <SFML/Audio.hpp>
#include <iostream>
#include "RhythmAccuracy.cpp"

using namespace std;

class AudioAnalyzer
{
public:

	AudioAnalyzer() {}

	AudioAnalyzer(string fileName)
	{
		this->fileName = fileName;
		setSampleInfo(fileName); //sample count, sample rate, and channel count
		samples = new short[sampleCount];
		loadSamples();
	}

	~AudioAnalyzer()
	{
		delete[] samples;
	}

	void setSampleInfo(string fileName) //delete once getTrimmedSampleCount is fixed
	{
		// load an audio buffer from a sound file
		buffer.loadFromFile(fileName);
		sampleCount = buffer.getSampleCount();
		sampleRate = buffer.getSampleRate();
		channelCount = buffer.getChannelCount();
	}

	short* getSamples()
	{
		return samples;
	}

	int getTrimmedSampleCount(string fileName)
	{
		sf::SoundBuffer buffer;
		buffer.loadFromFile(fileName);

		int totalSampleCount = buffer.getSampleCount();

		const short* bufferSamples = buffer.getSamples();
		//short* tempTrimmedSamples = new short[totalSampleCount];

		short emptySampleThreshold = 1; //if a sample is greater than 1, then it is not empty audio
		leadingEmptySamples = 0; //default value
		trailingEmptySamples = 0; //default value

		cout << "buffer.getSampleCount is " << buffer.getSampleCount() << endl;
		//find the amount of leading empty samples
		for (int count = 0; count < buffer.getSampleCount(); count++)
		{
			if (bufferSamples[count] > emptySampleThreshold)
			{
				leadingEmptySamples = count - 1;
				break;
			}
		}
		
		cout << "buffer.getSampleCount is " << buffer.getSampleCount() << endl;
		//find the amount of trailing empty samples
		//if count is of type sf::Uint64 instead of int, then things get screwy in this for loop
		for (int count = sampleCount - 1; count >= 0; count--)
		{	
			if (bufferSamples[count] > emptySampleThreshold)
			{
				trailingEmptySamples = sampleCount - count;
				break;
			}
		}



		trimmedSampleCount = (totalSampleCount - leadingEmptySamples) - trailingEmptySamples;

		return trimmedSampleCount;
	}

	sf::SoundBuffer getBufferAndSamples(string fileName)
	{
		buffer.loadFromFile(fileName);

		cout << "total sample count:" << buffer.getSampleCount() << endl;
		cout << "samplecount is " << sampleCount << endl;
		cout << "trimmed sample count:" << trimmedSampleCount << endl;
		//while(true){}
		cout << "sample rate:" << buffer.getSampleRate() << endl;
		cout << "channels:" << buffer.getChannelCount() << endl;
		cout << "duration:" << buffer.getDuration().asSeconds() << endl;

		

		const short* tempSamples = buffer.getSamples();
		unsigned int channelCount = buffer.getChannelCount();
		unsigned int sampleRate = buffer.getSampleRate();

		

		short minSample = SHRT_MAX;
		short maxSample = SHRT_MIN;
		int minSampleCount;
		int maxSampleCount;

		int checkpointCount = 25000;
		//load samples from buffer into samples[], starting from the last leading empty sample and ending
		//with the first trailing empty sample
		//the size of the samples array is already trimmed, which is why we can start at 0 in the for loop
		//but tempSamples is untrimmed which is why tempSamples[count + leadingEmptySamples] is used
		int debugcount = 0;
		for (int count = 0; count < sampleCount; count++)
		{
			debugcount++;

			/*

			if (samples[count] < minSample)
			{
				minSample = samples[count];
				minSampleCount = count;
			}
			if (samples[count] > maxSample)
			{
				maxSample = samples[count];
				maxSampleCount = count;
			}

			if (count % 25000 == 0 && count > 0)
			{
				cout << "checkpoint " << checkpointCount << ": minSample is " << minSample
					<< " at " << minSampleCount << " and maxSample is " << maxSample << " at " << maxSampleCount << endl;

				minSample = SHRT_MAX;
				maxSample = SHRT_MIN;
				checkpointCount += 25000;
			}

			*/

		}
		//cout << "samples[414156]: " << samples[414156] << endl;
		//cout << "samples[414157]: " << samples[414157] << endl;
		//cout << "samples[414158]: " << samples[414158] << endl;
		cout << "samples:" << samples << endl;
		cout << "debugcount is " << debugcount << endl;
		cout << "checkpoint" << endl;
		cout << "trimmed sample count:" << sf::Uint64(trimmedSampleCount) << endl;
		cout << "channels:" << buffer.getChannelCount() << endl;
		cout << "sample rate:" << buffer.getSampleRate() << endl;
		cout << "duration: " << buffer.getDuration().asSeconds() << endl;
		//cout << "channels:" << channelCount << endl;
		//cout << "sample rate:" << sampleRate << endl;

			

		buffer.loadFromSamples(samples, sampleCount, channelCount, sampleRate);

		return buffer;
	}



	RhythmAccuracy loadRhythmAccuracy(int bpm, string shortestNote)
	{
		RhythmAccuracy rAcc;
		rAcc.setSamples(getSamples());
		rAcc.setSampleInfo(sampleCount, sampleRate, channelCount);
		rAcc.setLeadingEmptySamples(leadingEmptySamples);
		rAcc.setTrailingEmptySamples(trailingEmptySamples);
		rAcc.setBpm(bpm);
		rAcc.setShortestNote(shortestNote);

		return rAcc;

	}

//private:

	void loadSamples()
	{
		const short* tempSamples = buffer.getSamples();
		for (int count = 0; count < sampleCount; count++)
		{
			//load samples from buffer into samples[]
			samples[count] = tempSamples[count];
		}
	}

	short* samples;
	long long sampleCount;
	int sampleRate;
	int channelCount;

	sf::SoundBuffer buffer;

	string fileName;
	int leadingEmptySamples;
	int trailingEmptySamples;
	int trimmedSampleCount;


};