#include "asm.h"

	.globl start
start:
	syscall0 NR_fork
	bnez $a0, pause

	b refresh	# proc-refresh

pause:
	syscall0 NR_pause
	b pause

refresh:
	syscall1_a NR_refresh, buffer
	syscall1_li NR_timer,REFRESH_SPEED
	b refresh

buffer:
	.ascii "  *                                                                                                                                                          *  "
	.ascii "*****                                                                                                                                                      *****"
	.ascii "  *                                                                                                                                                          *  "
	.ascii " * *                                                                                                                                                        * * "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                      *************                   *                                     *                 *            *                                    "
	.ascii "                                  *                   *          *********                  *                 *            *                                    "
	.ascii "                                  *    *         ***********     *       *         *******************        *****   ************                              "
	.ascii "                                  *  *              * * *        *       *                  *                 *            *                                    "
	.ascii "                                  **               *  *  *       *       *                 * *                *            *   *                                "
	.ascii "                                  *  *            *   *   *      *       *                *   *           *********         * *                                 "
	.ascii "                                   *   *         *    *    *     *       *               *     *          *       *          *                                  "
	.ascii "                                    *                 *          *       *              *       *         *       *        *  *                                 "
	.ascii "                                     *  *             *         *         *  *         *         *        *       *      *     *  *                             "
	.ascii "                                      *               *        *           *          *           *       *********             *                               "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "                                                                                                                                                                "
	.ascii "  *                                                                                                                                                          *  "
	.ascii "*****                                                                                                                                                      *****"
	.ascii "  *                                                                                                                                                          *  "
	.ascii " * *                                                                                                                                                        * * "
