#include <stdio.h> 
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sysexits.h>

void myown_stat(char *file){
	struct stat info;
	char modestr[11];
	if(stat(file, &info) == -1){
		perror(file);
	}else{
		struct stat *pointinfo;
		pointinfo = &info;
		int mode = pointinfo->st_mode;
		//show the file details
		//obtain mode string
		strcpy(modestr, "----------");
		if(S_ISDIR(mode)) modestr[0] = 'd';
		if(S_ISREG(mode)) modestr[0] = '-';
		if(S_ISLNK(mode)) modestr[0] = 'l';
		if(S_ISFIFO(mode)) modestr[0] = 'f';
		if(S_ISCHR(mode)) modestr[0] = 'c';
		if(S_ISBLK(mode)) modestr[0] = 'b';
		if(S_ISSOCK(mode)) modestr[0] = 's';
		
		if(mode & S_IRUSR) modestr[1] = 'r';
		if(mode & S_IWUSR) modestr[2] = 'w';
		if(mode & S_IXUSR) modestr[3] = 'x';

		if(mode & S_IRGRP) modestr[4] = 'r';
                if(mode & S_IWGRP) modestr[5] = 'w';
                if(mode & S_IXGRP) modestr[6] = 'x';

		if(mode & S_IROTH) modestr[7] = 'r';
                if(mode & S_IWOTH) modestr[8] = 'w';
                if(mode & S_IXOTH) modestr[9] = 'x';

		printf("%s"	, modestr );
		printf("%2d "	, (int)pointinfo->st_nlink);

		//uid to name
		struct passwd *getpwid(), *pw_ptr;
		pw_ptr = getpwuid(pointinfo->st_uid);
		char *temp = pw_ptr->pw_name;
		printf("%-6s "	, temp);

		// gid to name
		struct group *getgrgid(), *grp_ptr;
		grp_ptr = getgrgid(pointinfo->st_gid);
		temp = grp_ptr->gr_name;
		printf("%-6s "	, temp);

		//print size
		printf("%6ld "	, (long)pointinfo->st_size);	

		// ctime
		//printf("%.12s "	, 4 + ctime(&pointinfo->st_mtim.tv_sec));
		time_t rawtime;
		struct tm *info;
		char timebuff[80];
		time(&rawtime);
		info = localtime(&rawtime);
		int yearone = info->tm_year;
		struct tm *infotwo = localtime(&pointinfo->st_mtim.tv_sec);
		int yeartwo = infotwo->tm_year;

		if(yearone == yeartwo){
			printf(" %.12s "	, 4+ ctime(&pointinfo->st_mtim.tv_sec));
		}else{
			char *monthstr;
			if(infotwo->tm_mon == 0){
				monthstr = "Jan";
			}else if(infotwo->tm_mon == 1){
				monthstr = "Feb";
			}else if(infotwo->tm_mon == 2){
				monthstr = "Mar";
			}else if(infotwo->tm_mon == 3){
				monthstr = "Apr";
			}else if(infotwo->tm_mon == 4){
				monthstr = "May";
			}else if(infotwo->tm_mon == 5){
				monthstr = "Jun";
			}else if(infotwo->tm_mon == 6){
				monthstr = "Jul";
			}else if(infotwo->tm_mon == 7){
				monthstr = "Aug";
			}else if(infotwo->tm_mon == 8){
				monthstr = "Sep";
			}else if(infotwo->tm_mon == 9){
				monthstr = "Oct";
			}else if(infotwo->tm_mon == 10){
				monthstr = "Nov";
			}else{
				monthstr = "Dec";
			}
			int year = 1900 + infotwo->tm_year;
			sprintf(timebuff, " %s %d  %d ", monthstr, infotwo->tm_mday, year);
			printf("%s", timebuff);
		}
		

		//print filename
		bool findslash = false;
		int filepath_len = strlen(file); 
		int j, k, index;
		for(j = 0; j < filepath_len; j++){
			if(file[j] == '/'){
				findslash = true;
				index = j;
			}
		}
		if(findslash == true){
			for(k = index+1; k < filepath_len; k++){
                		printf("%c", file[k]);
                	}
                	printf("\n");
		}else{
			printf("%s\n"	, file);
		}
	}
}

void my_recurs(char *d_path, bool hidden, bool longmode){

	char *dd_path, *ddd_path;
	DIR *dirr;
	struct dirent *dppp;
	size_t n_len, p_len;
	struct stat sbbb;
	char *direct[100];
	int directories = 0;
	direct[0] = d_path;

		dirr = opendir(direct[0]);
        	if(dirr == NULL){
         		perror(d_path);
                	exit(1);
        	}else{
                	while((dppp=readdir(dirr)) != NULL){
                                p_len = strlen(direct[0]);
                                n_len = strlen(dppp->d_name);
                                dd_path=malloc(p_len + n_len + 2);
                                strcpy(dd_path, direct[0]);
                                dd_path[p_len] = '/';
                                strcpy(dd_path + p_len + 1, dppp->d_name);
				stat(dd_path, &sbbb);
				if(S_ISDIR(sbbb.st_mode)){
					if(dppp->d_name[0] != '.' && strcmp(dppp->d_name, ".") != 0 && strcmp(dppp->d_name, "..") != 0){
						directories++;
						direct[directories] = dppp->d_name;
					}
					if(hidden == true){
                         	                if(longmode == true){
                                	                myown_stat(dd_path);
                                        	}else{
                                                	printf("%s\t\n", dppp->d_name);
                                        	}
                                	}else{
                                        	if(dppp->d_name[0] != '.' && strcmp(dppp->d_name, ".") != 0 && strcmp(dppp->d_name, "..") != 0){
                                                	if(longmode == true){
                                                        	myown_stat(dd_path);
                                                	}else{
                                                        	printf("%s\t\n", dppp->d_name);
                                                	}
                                        	}
                                	}
				}else{
								
                			if(hidden == true){
                        			if(longmode == true){
                                			myown_stat(dd_path);
                                		}else{	
                                        		printf("%s\t\n", dppp->d_name);
                                		}
                        		}else{
                                		if(dppp->d_name[0] != '.' && strcmp(dppp->d_name, ".") != 0 && strcmp(dppp->d_name, "..") != 0){
                                        		if(longmode == true){
                                                		myown_stat(dd_path);
                                        		}else{	
                                        			printf("%s\t\n", dppp->d_name);
                                			}
                        			}
                    			}
        			}
			}
   		}closedir(dirr);
		int r;
		for(r = 1; r<=directories; r++){
			p_len = strlen(direct[0]);
			n_len = strlen(direct[r]);
			ddd_path=malloc(p_len + n_len + 2);
                        strcpy(ddd_path, direct[0]);
                        ddd_path[p_len] = '/';
                        strcpy(ddd_path + p_len + 1, direct[r]);
			printf("\n%s:\n", ddd_path);
			my_recurs(ddd_path, hidden, longmode);
		}
}

int main(int argc, char *argv[]) {
	char opt = 'a';
	bool hidden = false;
	bool dirasfile = false;
	bool longmode = false;
	bool recursive = false;
	int i = 0;
	DIR *dir;
	struct dirent *dpp;
	char *file, *d_path;
	size_t path_len, name_len;
	extern int optind; 	


	while((opt = getopt(argc, argv, "adlRst")) >= 0) {
        	switch(opt) {
                	case 'a':
				hidden = true;
        	                break;
                        case 'd':
				dirasfile = true;
                                break;        
                        case 'l':
				longmode = true;
                                break;
			case 'R':
				recursive = true;
				break;
			case 's':
				EX_USAGE;
				break;
			case 't':
				EX_USAGE;
				break;
                }
	}

	int directories = argc - optind;

	if(directories == 0) {
		//then we show the current directory
		dir = opendir(".");
		if(dir == NULL){
			perror("\n unable to open current directory\n");
			exit(1);	
		}else if(dirasfile == true){
			if(longmode == true){
				file = ".";
				myown_stat(file);
			}else{
				printf(".\n");
			}
		}else{
			while((dpp=readdir(dir)) != NULL){
				file = ".";
                                path_len = strlen(file);
                                name_len = strlen(dpp->d_name);
                                d_path=malloc(path_len + name_len + 2);
                                strcpy(d_path, file);       
                                d_path[path_len] = '/';
                                strcpy(d_path + path_len + 1, dpp->d_name);

				struct stat sbb;
				stat(d_path, &sbb);
				if(S_ISDIR(sbb.st_mode) && recursive == true && strcmp(dpp->d_name, ".") != 0 && strcmp(dpp->d_name, "..") != 0){
					printf("\n%s:\t\n", dpp->d_name);
					//   	CALL RECURSE
					my_recurs(d_path, hidden, longmode);
				}else{
					if(hidden == true){                
						if(longmode == true){
							myown_stat(dpp->d_name);
                                                }else{
                                                        printf("%s\t\n", dpp->d_name);
                                                }
                                        }else{
						if(dpp->d_name[0] != '.' && strcmp(dpp->d_name, ".") != 0 && strcmp(dpp->d_name, "..") != 0){
							if(longmode == true){
								myown_stat(dpp->d_name);
                                                	}else{
                                                        	printf("%s\t\n", dpp->d_name);
                                                	}
                                                }
                                        }
				}
			}
		}
	}else{
		//we show the directory(ies) provided 
		for(i = optind; i < argc; i ++){
			if(directories != 1 && dirasfile == false){
				printf("%s:\n", argv[i]);
			}
			dir = opendir(argv[i]);
			if(dir == NULL){
	                        perror(argv[i]);
        	                exit(1);
                	}else if(dirasfile == true){
				dir = opendir(".");
				if(dir == NULL){
					perror(".");
					exit(1);
				}else{
					while((dpp=readdir(dir)) != NULL){
						if(!(strcmp(dpp->d_name, argv[i]))){
							if(longmode == true){
								myown_stat(dpp->d_name);
							}else{
								printf("%s\t\n", argv[i]);	
							}
						}
					}continue;
				}
			}else{
                        	while((dpp=readdir(dir)) != NULL){
                        	        path_len = strlen(argv[i]);
                	                name_len = strlen(dpp->d_name);
        	                        d_path=malloc(path_len + name_len + 2);
   	                        	strcpy(d_path, argv[i]);
                        	        d_path[path_len] = '/';
                	                strcpy(d_path + path_len + 1, dpp->d_name);
	
                                      	struct stat sbb;
                                       	stat(d_path, &sbb);

                                       	if(S_ISDIR(sbb.st_mode) && recursive == true && strcmp(dpp->d_name, ".") != 0 && strcmp(dpp->d_name, "..") != 0){
						if(longmode == true){
							myown_stat(d_path);
						}else{
							printf("%s\n", dpp->d_name);
						}
                                               	printf("\n%s:\t\n", d_path);
						my_recurs(d_path, hidden, longmode);
						printf("\n");
                                       	}else{                
                                               	if(hidden == true){
                                                       	if(longmode == true){
								path_len = strlen(argv[i]);
								name_len = strlen(dpp->d_name);
								d_path=malloc(path_len + name_len + 2);
								strcpy(d_path, argv[i]);
								d_path[path_len] = '/';
								strcpy(d_path + path_len + 1, dpp->d_name);
								myown_stat(d_path);
                                                       	}else{
                                                               	printf("%s\t\n", dpp->d_name);
                                                       	}
                                               	}else{
                                                       	if(dpp->d_name[0] != '.' && strcmp(dpp->d_name, ".") != 0 && strcmp(dpp->d_name, "..") != 0){
								if(longmode == true){
									path_len = strlen(argv[i]);
									name_len = strlen(dpp->d_name);
									d_path = malloc(path_len + name_len + 2);
									strcpy(d_path, argv[i]);
									d_path[path_len] = '/';
									strcpy(d_path + path_len + 1, dpp->d_name);
									myown_stat(d_path);
        	                                                }else{
                                                                       	printf("%s\t\n", dpp->d_name);
                                                               	}
                                                        }
                                        	}
                                	}
                        	}
                	}printf("\n");
		}
        }
	closedir(dir);
	return 0;
}
