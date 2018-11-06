#include <stdio.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
	char thisEnv[16384];
	printf("{");
	for (char **env = envp; *env != 0; env++)
	{
		strncpy(thisEnv,*env,sizeof(thisEnv));
		thisEnv[16383] = 0;
		if (thisEnv) {
			char *eqp = strchr(thisEnv,'=');
			if (eqp) {
				*eqp++ = 0;
				char *tmp = eqp;
				while (*tmp) {
					if ((*tmp == '"')||(*tmp == '\'')||(*tmp == '\\')||(*tmp == '/')||(*tmp == '~'))
						*tmp = '_';
				++tmp;
				}
				printf("\"%s\": \"%s\",",thisEnv,eqp);
			}
		}
	}
	printf("\"~\": \"%s\"}\n",(argc > 1) ? argv[1] : "");
	return 0;
}
