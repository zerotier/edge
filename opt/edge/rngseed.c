/*
 * Seed the linux PRNG from a file containing random data
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <linux/random.h>

struct entropy {
	int ent_count;
	int size;
	unsigned char data[512];
};

int main(int argc, char **argv) {
	int rand_fd;
	int entropy_fd;
	int ret = EXIT_FAILURE;

	rand_fd = open("/dev/random", O_RDWR);
	if (rand_fd < 0) {
		perror("Can't open /dev/random");
		goto no_rand;
	}

	if (argc > 1) {
		entropy_fd = open(argv[1], O_RDONLY);
		if (entropy_fd < 0) {
			perror("Can't open entropy file");
			goto no_entropy;
		}
	} else {
		entropy_fd = 0;
	}

	for (;;) {
		struct entropy entropy;

		entropy.size = read(entropy_fd, entropy.data, sizeof(entropy.data));

		if (entropy.size < 0) {
			perror("Error while reading entropy file");
			goto entropy_read_failed;
		}

		if (entropy.size == 0) {
			/* Done */
			break;
		}

		/* Assume each byte has 8 bits of entropy */
		entropy.ent_count = entropy.size * 8;

		if (ioctl(rand_fd, RNDADDENTROPY, &entropy) < 0) {
			perror("RNDADDENTROPY failed");
		}
	}

	ret = EXIT_SUCCESS;

 entropy_read_failed:
	close(entropy_fd);
 no_entropy:
	close(rand_fd);
 no_rand:
 no_arg:
	return ret;
}
