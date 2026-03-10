#pragma once

#include "Common.hpp"
#include <iomanip>

const char* path = "Resources\\AnchorData.txt";

std::streamoff todayPos = -1;

//if the last date is the current one, it does nothing, otherwise, it writes a new date block
void checkDate()
{
	std::ifstream in(path, std::ios::binary); //binary access, doesn't have anything to do with file being ascii
	if (!in) return;

	in.seekg(0, std::ios::end); //reads from the end
	std::streamoff pos = in.tellg(); //current read position, past the last byte
	const std::streamoff fileSize = pos;

	std::string lastDateLine; //will contain the date
	std::string line;

	//looks for a "d" and retrieves the date
	while (pos > 0)
	{
		--pos; // move one byte backward
		in.seekg(pos); // place cursor exactly at pos

		char c = in.peek(); // reads the byte at pos

		// \n marks the end of a line, pos = 0 the start of the file
		if (c == '\n' || pos == 0)
		{
			// if we are between lines, d will be in the first position
			if (c == '\n')
			{
				if (pos + 1 >= fileSize) continue; //if you are at the end of the file, jump to next --pos or it will bug out
				in.seekg(pos + 1);
			}
			else
			{
				in.seekg(0);
			}

			std::streamoff lineStartPos = in.tellg(); //we retrieve the pos before getline because it moves it +1
			std::getline(in, line);
			// ending a line in a windows text file is actually a \r\n, we remove that r
			if (!line.empty() && line.back() == '\r') line.pop_back();

			if (!line.empty() && line[0] == 'd')
			{
				lastDateLine = line;
				todayPos = lineStartPos; //this is the case where the date already existed
				break;
			}
		}
	}
	in.close();



	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);

	std::ostringstream ss;
	ss << 'd' << std::put_time(&tm, "%Y%m%d");

	if (lastDateLine == ss.str())
		return;

	// saving the pos before adding the new block
	std::ifstream fin(path, std::ios::binary);
	fin.seekg(0, std::ios::end);
	todayPos = fin.tellg();
	fin.close();

	std::ofstream out(path, std::ios::binary | std::ios::app);
	if (!out) return;

	out << ss.str() << '\n'
		<< "1-0" << '\n'
		<< "2-0" << '\n'
		<< "3-0" << '\n';
}

void checkStopwatch(const std::string& id)
{
	std::ifstream in(path, std::ios::binary);
	if (!in || todayPos < 0) return;

	in.seekg(todayPos);

	std::string line;

	// skip the date line
	if (!std::getline(in, line)) return;

	while (std::getline(in, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		// stop if we reached the next date block
		if (!line.empty() && line[0] == 'd')
			return;

		// match stopwatch id
		if (line.rfind(id, 0) == 0) // starts with id
		{
			std::cout << line << std::endl;
			return;
		}
	}
}


void readTodaysBlock(std::string& todaysBlock)
{
	todaysBlock.clear();

	if (todayPos < 0) return;

	std::ifstream in(path, std::ios::binary);
	if (!in) return;

	// Calculating filesize
	in.seekg(0, std::ios::end);
	std::streamoff fileSize = in.tellg();

	if (todayPos >= fileSize)
		return;

	// Read todays block
	std::streamoff remaining = fileSize - todayPos;
	todaysBlock.resize(static_cast<size_t>(remaining));

	in.seekg(todayPos);
	in.read(&todaysBlock[0], todaysBlock.size());

	in.close();

	cout << "readTodaysBlock: " << '\n' << todaysBlock << '\n';
}

// This function does NOT read today's block, it only truncates the file to keep the prefix.
void extractTodaysBlock()
{
	if (todayPos < 0) return;

	std::ifstream in(path, std::ios::binary);
	if (!in) return;

	// Calculating filesize
	in.seekg(0, std::ios::end);
	std::streamoff fileSize = in.tellg();

	if (todayPos >= fileSize)
		return;

	// getting everything before todayPos, file will be rewrited with only prefix
	in.seekg(0);
	std::string prefix;
	prefix.resize(static_cast<size_t>(todayPos));
	in.read(&prefix[0], prefix.size());

	in.close();

	// Truncate file and write prefix back
	std::ofstream out(path, std::ios::binary | std::ios::trunc);
	if (!out) return;

	out.write(prefix.data(), prefix.size());

	cout << "extracted" << '\n' << '\n';
}

void appendTodaysBlock(const std::string& todaysBlock)
{
	if (todaysBlock.empty()) return;

	std::ofstream out(path, std::ios::binary | std::ios::app);
	if (!out) return;

	out.write(todaysBlock.data(), todaysBlock.size());
	cout << "appendedTodaysBlock: " << '\n' << todaysBlock << '\n';

}

//hacer que se compruebe todo el bloque, por si hay más de un "-"
//si solo hay uno y el id coincide con el endStopwatch, todo correcto, no es error
bool isAnythingRunning(std::string& todaysBlock)
{
	for (size_t i = 0; i < todaysBlock.size(); ++i)
	{
		if (todaysBlock[i] == 's')
		{
			// scan backwards on the same line
			size_t j = i;
			while (j > 0 && todaysBlock[j - 1] != '\n')
			{
				--j;
				if (todaysBlock[j] == '-' && j > 0)
				{
					char beforeDash = todaysBlock[j - 1];
					cout << "Error: A stop watch is arready started, id: " << beforeDash << endl;
					return 1;
				}
			}
		}
	}
	return 0;
}

//No edge cases comprobations, assumes there's only one s in the correct ID
//MIDNIGHT CROSSINGS ERROR
void endStopwatch(std::string& todaysBlock)
{
	size_t sPos = std::string::npos;
	for (size_t i = 0; i < todaysBlock.size(); ++i)
	{
		if (todaysBlock[i] == 's')
		{
			sPos = i;
			break;
		}
	}
	if (sPos == std::string::npos) return;

	//read int after s (after this int will always be a space, or a \r\n)
	//this int is HHMMSS, calculates the difference between the 2
	int startSeconds =
	std::stoi(todaysBlock.substr(sPos + 1, 2)) * 3600 + //stoi = string to int
		std::stoi(todaysBlock.substr(sPos + 3, 2)) * 60 +
		std::stoi(todaysBlock.substr(sPos + 5, 2));

	std::time_t t = std::time(nullptr);
	std::tm tm{};
	localtime_s(&tm, &t);

	int diff =(tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec) - startSeconds;


	// returns the closest "-" searching backward
	size_t dash = todaysBlock.rfind('-', sPos);

	// getting the accumulated and adding to it diff
	size_t accStart = dash + 1;
	int accumulated = std::stoi(todaysBlock.substr(accStart, sPos - accStart));
	accumulated += diff;


	// replacing the line only with "id-accumulated"
	size_t lineStart = todaysBlock.rfind('\n', dash);
	lineStart = (lineStart == std::string::npos) ? 0 : lineStart + 1;

	size_t lineEnd = todaysBlock.find('\n', sPos);
	if (lineEnd == std::string::npos) lineEnd = todaysBlock.size();

	//replaces from lineStart to lineEnd with id- + accumulated
	todaysBlock.replace(
		lineStart,
		lineEnd - lineStart,
		todaysBlock.substr(lineStart, accStart - lineStart) + std::to_string(accumulated)
	);

	cout << "Ended stopwatch from "
		<< std::setw(2) << std::setfill('0') << (startSeconds / 3600) << ":"
		<< std::setw(2) << std::setfill('0') << ((startSeconds % 3600) / 60) << ":"
		<< std::setw(2) << std::setfill('0') << (startSeconds % 60)
		<< " to "
		<< std::setw(2) << std::setfill('0') << tm.tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << tm.tm_min << ":"
		<< std::setw(2) << std::setfill('0') << tm.tm_sec
		<< " (" << diff / 60 << " minutes)"
		<< std::endl;
}

void resumeStopwatch(const std::string& id, std::string& todaysBlock)
{
	if (isAnythingRunning(todaysBlock)) return;

	// getting current time HHMMSS and setting it into stamp
	std::time_t t = std::time(nullptr);
	std::tm tm{};
	localtime_s(&tm, &t);

	std::ostringstream ss;
	ss << " s" << std::put_time(&tm, "%H%M%S");
	const std::string stamp = ss.str();

	//position ater our accumulated value
	size_t posAfterAccum = std::string::npos;

	// scan line by line inside todaysBlock, modify in place
	size_t lineStart = 0;
	while (lineStart < todaysBlock.size())
	{
		size_t lineEnd = todaysBlock.find('\n', lineStart);
		if (lineEnd == std::string::npos) lineEnd = todaysBlock.size(); //it shouldn't enter here as we always have \n

		// effectiveEnd trims '\r'
		size_t effectiveEnd = lineEnd;
		if (effectiveEnd > lineStart && todaysBlock[effectiveEnd - 1] == '\r')
			--effectiveEnd;

		//checks if our line starts with the id
		if (todaysBlock.compare(lineStart, id.size(), id) == 0)
		{
			size_t j = lineStart + id.size();

			// move to end of accumulated value (digits)
			while (j < effectiveEnd && todaysBlock[j] >= '0' && todaysBlock[j] <= '9')
				++j;

			posAfterAccum = j;
			break;
		}

		if (lineEnd == todaysBlock.size()) break;
		lineStart = lineEnd + 1;
	}

	todaysBlock.insert(posAfterAccum, stamp);
}



void debugPrintTodayLine()
{
	std::ifstream in(path, std::ios::binary);
	if (!in || todayPos < 0)
	{
		std::cout << "error opening or reading file" << endl;
		return;
	}

	in.seekg(todayPos);

	std::string line;
	std::getline(in, line);

	if (!line.empty() && line.back() == '\r')
		line.pop_back();

	std::cout << line << std::endl;
}

// Reads the accumulated value of the given id from today's block into acc
void sync(const std::string& id, float& acc)
{
	acc = 0.0f;

	std::ifstream in(path, std::ios::binary);
	if (!in || todayPos < 0) return;

	in.seekg(todayPos);

	std::string line;

	// skip date line
	if (!std::getline(in, line)) return;

	while (std::getline(in, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		// stop if next date block
		if (!line.empty() && line[0] == 'd')
			return;

		// match stopwatch id
		if (line.rfind(id, 0) == 0)
		{
			acc = std::stof(line.substr(id.size()));
			return;
		}
	}
}
