#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;

class AudioAnalyzer
{
public:

	AudioAnalyzer() {}

	AudioAnalyzer(string fileName)
	{
		this->fileName = fileName;
	}

	int getSampleCount(string fileName) //delete once getTrimmedSampleCount is fixed
	{
		sf::SoundBuffer buffer;
		buffer.loadFromFile(fileName);
		return buffer.getSampleCount();
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
		for (int count = buffer.getSampleCount() - 1; count >= 0; count--)
		{	
			if (bufferSamples[count] > emptySampleThreshold)
			{
				trailingEmptySamples = buffer.getSampleCount() - count;
				break;
			}
		}



		trimmedSampleCount = (totalSampleCount - leadingEmptySamples) - trailingEmptySamples;

		return trimmedSampleCount;
	}

	sf::SoundBuffer getBufferAndSamples(string fileName, short* &samples)
	{

		// load an audio buffer from a sound file
		sf::SoundBuffer buffer;
		buffer.loadFromFile(fileName);

		cout << "total sample count:" << buffer.getSampleCount() << endl;
		cout << "trimmed sample count:" << trimmedSampleCount << endl;
		//while(true){}
		cout << "sample rate:" << buffer.getSampleRate() << endl;
		cout << "channels:" << buffer.getChannelCount() << endl;
		cout << "duration:" << buffer.getDuration().asSeconds() << endl;

		

		const short* tempSamples = buffer.getSamples();
		long long sampleCount = buffer.getSampleCount();
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
		for (int count = 0; count < trimmedSampleCount; count++)
		{
			debugcount++;
			//load samples from buffer into samples[]
			samples[count] = tempSamples[count + leadingEmptySamples];

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

		}
		cout << samples[414157] << endl;
		cout << "debugcount is " << debugcount << endl;
		cout << "checkpoint" << endl;
		cout << "trimmed sample count:" << sf::Uint64(trimmedSampleCount) << endl;
		cout << "sample rate:" << buffer.getSampleRate() << endl;
		cout << "channels:" << buffer.getChannelCount() << endl;

		buffer.loadFromSamples(samples, sf::Uint64(trimmedSampleCount), channelCount, sampleRate);

		while (true) {}

		return buffer;
	}

private:

	string fileName;
	int leadingEmptySamples;
	int trailingEmptySamples;
	int trimmedSampleCount;


};