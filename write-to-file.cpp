/**
 * @brief Program to add include guards in all C++ headers.
 */
#include <cerrno>
#include <fstream>
#include <iostream>
#include <vector>

#define PRAGMA "#pragma once"
#define PRAGMA_SIZE sizeof(PRAGMA) - 1 //Subtracting 1 to exclude '\0' character
#define IFNDEF "#ifndef "
#define IFNDEF_SIZE sizeof(IFNDEF)
#define DEFINE "#define "
#define DEFINE_SIZE sizeof(DEFINE)
#define ENDIF  "\n#endif // "
#define ENDIF_SIZE sizeof(ENDIF)
#define MAX_HEADER_SIZE 120			  //Number chosen arbitrarily

typedef enum {
	IS_IFNDEF,
	IS_DEFINE,
	IS_ENDIF
} Type;

/**
 * @brief Copy input header to an empty string to create proper include guards. 
 *
 * @param[in] src    	  Input header
 * @param[in] dst    	  String to be copied to
 * @param[in] length 	  Length of header
 * @param[in] header_type Type of header to be appended to the string
 *
 * @return status.
 */
static inline int header_strncpy(char const* &src,
								 char * &dst,
								 int length,
								 Type header_type)
{
	int status = 0;
	switch (header_type) {
		//Creates guard with #ifndef src
		case IS_IFNDEF:
			std::strncpy(dst, IFNDEF, IFNDEF_SIZE);
			//-1 to start the copying from index of '\0' in "#ifndef "
			std::strncpy(dst + IFNDEF_SIZE - 1, src, length);
			dst[IFNDEF_SIZE + length - 1] = '\0';
			break;
		//Creates guard with #define src
		case IS_DEFINE:
			std::strncpy(dst, DEFINE, DEFINE_SIZE);
			std::strncpy(dst + DEFINE_SIZE - 1, src, length);
			dst[DEFINE_SIZE + length - 1] = '\0';
			break;
		//Creates guard with #endif // src
		case IS_ENDIF:
			std::strncpy(dst, ENDIF, ENDIF_SIZE);
			std::strncpy(dst + ENDIF_SIZE - 1, src, length);
			dst[ENDIF_SIZE + length - 1] = '\0';
			break;
		default:
			std::cerr << "Wrong type chosen\n";
			status = EINVAL;
			break;
	}

	return status;
}

/**
 * @brief Overwrite the header file with added include guards.
 *
 * @param[in] file_obj File object to be written to
 * @param[in] records  Records in the original header file.
 */
static inline void overwriteFile(std::ofstream &file_obj,
								 std::vector<std::string> &records)
{
	for (int i = 0; i < records.size(); i++) {
		file_obj << records[i] << "\n";
	}
}

/**
 * @brief Add include guards critical function to the file records
 *
 * @param[in] file_obj 		Read-only file object.
 * @param[in] records  		Lines/records in the file.
 * @param[in] filename 		Header file to read.
 * @param[in] include_guard Include guard to add.
 *
 * @return status
 */
static int addIncludeGuardInternal(std::ifstream &file_obj,
								   std::vector<std::string> &records,
		    					   char const * &filename,
								   char const *include_guard)
{
	int status = 0;
	std::string line;
	unsigned int number = 0U;
	bool flag = false;
	int length = strlen(include_guard);

	if ((length > MAX_HEADER_SIZE) || (length < 0)) {
		std::cerr << "Header name exceeds max permissible length\n";
		status = EBADMSG;
		goto end;
	}

	//Read all lines in the file
	while (std::getline(file_obj, line)) {
		const char *c_line = line.c_str();
		if ((!flag) && (std::strncmp(PRAGMA, c_line, PRAGMA_SIZE) == 0)) {
			flag = true;
			char *preprocesser = new char[length + IFNDEF_SIZE ];
			status = header_strncpy(include_guard, preprocesser, length, IS_IFNDEF);
			if (status == 0) {
				records.push_back(std::string(preprocesser));
				status = header_strncpy(include_guard, preprocesser, length, IS_DEFINE);
				if (status == 0) {
					records.push_back(std::string(preprocesser));
				}
			}
			delete[] preprocesser;
		}
		records.push_back(line);
	}

	if (!flag) {
		std::cerr << "No pragma found in file. Not adding include guard.\n";
		status = 0;
	} else {
		char *endif_ = new char[length + ENDIF_SIZE];
		status = header_strncpy(include_guard, endif_, length, IS_ENDIF);
		if (status == 0) {
			records.push_back(std::string(endif_));
		}
		delete[] endif_;
	}

end:
	file_obj.close();
	return status;
}

/**
 * @brief Add include guards public function.
 *
 * @param[in] filename 		File to add incldue guards to.
 * @param[in] incldue_guard Include guard to be added.
 *
 * @return status
 */
int addIncludeGuard(char const *filename, char const *include_guard) {
	int status = 0;
	std::ifstream file_obj;
	std::vector<std::string> file_records;

	file_obj.open(filename, std::ios::in); 

	if (file_obj.is_open()) {
		status = addIncludeGuardInternal(file_obj, file_records, filename, include_guard);
		if (file_obj.is_open()) {
			std::cerr << "Not able to close file\n";
			status = ENOTTY;
		 } else {
			 std::ofstream outfile_obj;
			 outfile_obj.open(filename, std::ios::trunc);
			 overwriteFile(outfile_obj, file_records);
			 outfile_obj.close();
		 }
	} else {
		std::cerr << "No such file exists\n";
		status = ENOENT; 
	}

	return status;
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cerr << "Wrong usage: needs a file name and header to add\n";
		exit(1);
	}
	if (argc == 2) {
		std::cerr << "Wrong usage: needs a header string to add to file\n";
		exit(1);
	}
	if (argc > 3) {
		std::cerr << "Wrong usage: Only needs a single include guard arg.\n";
	}

	printf("Calling %s, for file: %s\n", __FILE__, argv[1]);
	int ret_status = addIncludeGuard(argv[1], argv[2]);
	if (ret_status != 0) {
		std::cerr << "addIncludeGuard failed: filename: " << argv[1] << "\n";
	}
	return ret_status;
}
