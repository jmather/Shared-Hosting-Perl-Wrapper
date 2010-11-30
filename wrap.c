#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <pwd.h>            /* getpwuid         */
#include <sys/types.h>

void print_user_info() {
  
   struct passwd *passwd;
                    /* Get the uid of the running
                     * process and use it to get
                     * a record from /etc/passwd
                     */
   passwd=getpwuid(getuid());

  // Log UID/eUID of call!
  syslog(LOG_INFO, "Perl called. User: %s  UID: %d  eUID: %d  GID: %d\n", passwd->pw_gecos, getuid(), geteuid(), getgid());
}

void print_environment(char* envp[]) {
  int i = 0;
  int size = 0;
  int length = 0;
  while(envp[size] != NULL)
  {
    length += strlen(envp[size]) + 1;
    size++;
  }
  char env_buffer[length];
  env_buffer[0] = 0;
  for (i = 0; i < size; i++)
  {
    sprintf(env_buffer, "%s %s", env_buffer, envp[i]);
  }
  syslog(LOG_INFO, "Environment:%s\n", env_buffer);
}

void print_command_line(int argc, const char* argv[]) {
  int i = 0;
  int output_size = 0;
  for (i = 0; i < argc; i++)
  {
    output_size += strlen(argv[i]) + 1;
  }
  char cmdline_buffer [output_size];
  cmdline_buffer[0] = 0;
  for (i = 0; i < argc; i++)
  {
    sprintf(cmdline_buffer, "%s %s", cmdline_buffer, argv[i]);
  }
  syslog(LOG_INFO, "Command line:%s\n", cmdline_buffer);
}

int check_working_directory() {
  int num_of_paths = 4;
  int i = 0;
  char *paths[4] = {"/tmp", "/usr/tmp", "/var/tmp", "/dev/shm"};
  char *result;
  char mycwd[1024];

  if (getcwd(mycwd, 1024))
  {
    syslog(LOG_INFO, "Current working directory: %s", mycwd);
    for (i = 0; i < num_of_paths; i++)
    {
      result = strstr(mycwd, paths[i]);
      if (result != NULL)
      {
        if (strcmp(result, mycwd) == 0)
        {
          syslog(LOG_ERR, "Refused to run with CWD of \"%s\"", mycwd);
          printf("Can't execute files from %s\n", result);
          return 1;
        }
      }
    }
  }
  return 0;
}

int main(int argc, const char* argv[], char* envp[]) {
  int i = 0;
  char mycwd[1024];
  // CHANGE THE LINE BELOW! Customize it! Diversity is security!
  const char* cmd = "/usr/bin/perl_real";

  if (getuid() == 0) {
    execv(cmd, argv);
    return 0;
  }

  openlog("perllog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL5);
  setlogmask(LOG_UPTO(LOG_DEBUG));

  print_user_info();  

  // Check CWD
  if (check_working_directory() == 1)
  {
    return 1;
  }

  //print_environment(envp);
  print_command_line(argc, argv);
  // Check command line options
  for( i = 0; i < argc; i++ )
  {
    char* result = strstr(argv[i], "/tmp/");
    if (result != NULL)
    {
      if (strcmp(argv[i], result) == 0)
      {
        syslog(LOG_ERR, "Refused to run \"%s\"", argv[i]);
        printf("Can't open perl script \"%s\": No such file or directory\n", result);
        return 1;
      }
    }
  }
  closelog();
  execv(cmd,argv);
  return 0;
}
       

