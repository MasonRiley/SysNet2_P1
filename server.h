#ifndef DEF_SERVER
#define DEF_SERVER

/**
 * readHTMLFile: Reads in the contents of a file one character at a time,
 * storing the results in dataBuff. Counts the number of chars for
 * Content-Length. Then concatenates the pre-formatted response header
 * (see line 19), the determined content-length, and the file data to
 * send to the client.
 * @Params fileName The name of the file being read in.
 */
void readHTMLFile(char *fileName);

void readImageFile(char *fileName);

void readErrorFile(char *fileName);

/**
 * getFiles: Reads in every file in the cwd and stores their names in
 * 'files'. Additionally tracks how many files are in the cwd.
 */
void getFiles();

/**
 * checkFileExists: Takes in a complete request header from the client  
 * and determines if the requested file from the client exists. If it 
 * exists, returns the index of the file in 'files', -1 otherwise.
 * @Params buff The buffer containing the entire request header
 * @Return the index of the file in 'files' if the file exists, -1
 * otherwise
 */
int checkFileExists(char *buff);

/**
 * contentType: Takes in a request string and detemines from it if the
 * server needs to return an image or a text file.
 * @Params request The string of the request.
 * @Return int Whether it is a png or an html request.
 */
int contentType(char *request);

int getUserAgent(char *request);

#endif
