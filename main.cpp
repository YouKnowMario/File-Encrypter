#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

void printHelp()
{
	std::cout << "  ______                             _" << std::endl;
	std::cout << " |  ____|                           | | " << std::endl;
	std::cout << " | |__   _ __   ___ _ __ _   _ _ __ | |_ ___ _ __" << std::endl;
	std::cout << " |  __| | '_ \\ / __| '__| | | | '_ \\| __/ _ \\ '__|" << std::endl;
	std::cout << " | |____| | | | (__| |  | |_| | |_) | ||  __/ |   " << std::endl;
	std::cout << " |______|_| |_|\\___|_|   \\__, | .__/ \\__\\___|_|   " << std::endl;
	std::cout << "                          __/ | |                 " << std::endl;
	std::cout << "                         |___/|_|                 " << std::endl;
	std::cout << std::endl;
	std::cout << "encrypt files and folders with password" << std::endl;
	std::cout << std::endl;
	
	std::cout << "usage: encrypter [options] [input file name] [password] [output file name]" << std::endl;
	std::cout << std::endl;
	std::cout << "for more info visit the docs - www.exmple.com" << std::endl;
	std::cout << "-h or --help		print this message" << std::endl;
	std::cout << "-e or --encrypt		encrypts the input file to the output file" << std::endl;
	std::cout << "-d or --decrypt		decrypts the input encrypted file or directory" << std::endl;
	std::cout << "-E			encrypts the input file and deletes it" << std::endl;
	std::cout << "-D			decrypts the input file or directory and deletes it" << std::endl;
	std::cout << "-ed			encrypts the input directory to the output file" << std::endl;
	std::cout << "-ED			encrypts the input directory to the output file and deletes it" << std::endl;


}

// encrypted file structure: (everything is encrypted)
// required | 4 bytes | format | "dir " for directory, "file" for single file
// optional (if directory) | 4 bytes | fileNum | number of files | 
// optional (if directory) | 4 bytes | direcNameSize | length of the name of the directory |
// optional (if directory) | direcNameSize | direcName | the name of the directory |
// required | | data | the data
// 
// data structure: if more than one file this is repeated [fileNum] times
// 4 bytes | fileNameSize | length of the file name
// fileNameSize | fileName | file name
// 4 bytes | fileSize | the size of the file (in bytes)
// fileSize | fileData | the data of the file
//


void encryptFile(std::string input, std::string output, std::string password, const int& mode)
{
	std::ofstream wf(output, std::ios::binary); // open the output file for write

	std::vector<std::string> files;
	// fill the vector and write file header:
	if (mode == 0 || mode == 1)
	{
		files.push_back(input);
		wf.write("file", sizeof(char) * 4);  // write the format
	}
	else if (mode == 2 || mode == 3)
	{
		for (std::filesystem::directory_entry name : std::filesystem::directory_iterator(input))
		{
			files.push_back(name.path().string());
		}
		unsigned i = files.size();
		wf.write("dir ", sizeof(char) * 4); // write the format
		wf.write((char*)&i, sizeof(unsigned)); // write the fileNum
		i = input.size();
		wf.write((char*)&i, sizeof(unsigned)); // write the direcNameSize
		wf.write(input.c_str(), i * sizeof(char)); // write the direcName
	}
	if (output == "")
		output = "encryptedFile";
	for (std::string filename : files) // write the data
	{
		unsigned fileSize;
		unsigned filenamelength;
		std::ifstream rf(filename, std::ios::binary); // open the file to encrypt
		if (!rf) { // check if exist
			std::cout << "Cannot open file!" << std::endl;
			return;
		}
		if (std::filesystem::file_size(filename) > 4294967295) // if file is more then 4 giga bytes
		{
			std::cout << "file is too big (only files up to 4 gb)" << std::endl;
			return;
		}
		fileSize = std::filesystem::file_size(filename);
		std::cout << fileSize << std::endl;
		filenamelength = filename.length();
		// write the data to the file
		wf.write((char*)&filenamelength, sizeof(unsigned)); // write the length of the name
		wf.write(filename.c_str(), filenamelength * sizeof(char)); // write the name
		wf.write((char*)&fileSize, sizeof(unsigned)); // write the file size
		// code for now (i will add encryption)
		char temp;
		while (rf.read(&temp, sizeof(char))) // write byte by byte to byte to the file
		{
			temp--;
			wf.write(&temp, sizeof(char));
		}
		rf.close(); // close the file
	}
	
	wf.close(); // closing the file
	
}

void decryptFile(std::string input, std::string output, std::string password, const int& mode)
{
	std::ifstream rf(input, std::ios::binary); // open the input file for read
	if (!rf) {
		std::cout << "Cannot open file!" << std::endl;
		return;
	}
	std::vector<std::string> files;
	char* format = new char[4];
	
	rf.read(format, sizeof(char) * 4);  // read the format
	unsigned NameLength;
	std::string name;
	rf.read((char*)&NameLength, sizeof(unsigned));  // read the name length
	name.resize(NameLength);
	rf.read(&name[0], NameLength);  // read the name
	//std::string name = tempName;
	std::cout << name << std::endl;
	if (std::filesystem::exists(name))
	{
		char answer;
		std::cout << "\'" << name << "\'" << " is already exists. Overwrite? [y/n] ";
		std::cin >> answer;
		if (answer == 'y')
		{
			std::filesystem::remove(name); // remove the directory
		}
		else
			std::cout << "Not overwriting - exists" << std::endl;
	}

	if (!strcmp(format, "dir ")) // the encryption is of directory
	{
		std::filesystem::create_directory(name); // create the directory
		std::cout << "decrypting directory" << std::endl;
		goto clean;
	}
	else if (strcmp(format, "file")) // the encryption is of file
	{
		std::ofstream wf(name, std::ios::binary); // open the output file to write
		if (!wf) {
			std::cout << "Cannot open file for writing!" << std::endl;
			goto clean;
		}
		unsigned fileSize;
		char byte;
		rf.read((char*)&fileSize, sizeof(unsigned)); // read the file size
		while (rf.read(&byte, 1))
		{
			byte++;
			wf.write(&byte, 1);
		}
		wf.close();
		rf.close();
		goto clean;
	}
	else // wrong encryption
	{
		std::cout << "wrong file encryption" << std::endl;
		goto clean;
	}
clean:

	delete[] format;
	return;

}
#ifdef _DEBUG // if debug
int main()
{
	int argc = 5;
	const char* argv[5] = { "encrypter", "-d", "encryptedImage", "lol", "hello_world2.png" };
#else 

int main(int argc, char* argv[])
{
#endif

	if (argc == 1)
		printHelp();
	const char* output;
	if (argc < 4)
		return -1;
	if (argc == 4)
		output = "";
	else
		output = argv[4];
	
	if (!strcmp(argv[1], "--encrypt") || !strcmp(argv[1], "-e"))
		encryptFile(argv[2], output, argv[3], 0);
	else if (!strcmp(argv[1], "--decrypt") || !strcmp(argv[1], "-d"))
		decryptFile(argv[2], output, argv[3], 0);
	else if (!strcmp(argv[1], "-D"))
		decryptFile(argv[2], output, argv[3], 1);
	else if (!strcmp(argv[1], "-E"))
		encryptFile(argv[2], output, argv[3], 1);
	else if (!strcmp(argv[1], "-ed"))
		encryptFile(argv[2], output, argv[3], 2);
	else if (!strcmp(argv[1], "-ED"))
		encryptFile(argv[2], output, argv[3], 3);
	else
		std::cout << "wrong parameter" << std::endl;

}