/*****************************************************************************\
 *  state_test.c - Test state of Blue Gene base partitions and switches. 
 *  DRAIN nodes in SLURM that are not usable. 
 *****************************************************************************
 *  Copyright (C) 2004 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Dan Phung <phung4@llnl.gov> and Morris Jette <jette1@llnl.gov>
 *  
 *  This file is part of SLURM, a resource management program.
 *  For details, see <http://www.llnl.gov/linux/slurm/>.
 *  
 *  SLURM is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *  
 *  SLURM is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with SLURM; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
\*****************************************************************************/

#include "bluegene.h"

#define BUFSIZE 4096

#ifdef HAVE_BGL_FILES
/* Find the specified BlueGene node ID and configure it down in CMCS */
static void _configure_node_down(rm_bp_id_t bp_id, rm_BGL_t *bgl)
{
	int bp_num, i, rc;
	rm_bp_id_t bpid;
	rm_BP_t *my_bp;
	rm_location_t bp_loc;
	rm_BP_state_t bp_state;
	char bgl_down_node[128];

	if ((rc = rm_get_data(bgl, RM_BPNum, &bp_num)) != STATUS_OK) {
		error("rm_get_data(RM_BPNum): %s", bgl_err_str(rc));
		bp_num = 0;
	}

	for (i=0; i<bp_num; i++) {
		if (i) {
			if ((rc = rm_get_data(bgl, RM_NextBP, &my_bp)) != 
					STATUS_OK) {
				error("rm_get_data(RM_NextBP): %s", 
					bgl_err_str(rc));
				continue;
			}
		} else {
			if ((rc = rm_get_data(bgl, RM_FirstBP, &my_bp)) != 
					STATUS_OK) {
				error("rm_get_data(RM_FirstBP): %s", 
					bgl_err_str(rc));
				continue;
			}
		}

		if ((rc = rm_get_data(my_bp, RM_BPID, &bpid)) != STATUS_OK) {
			error("rm_get_data(RM_BPID): %s", bgl_err_str(rc));
			continue;
		}
		if (strcmp(bp_id, bpid) != 0)	/* different base partition */
			continue;
		if ((rc = rm_get_data(my_bp, RM_BPLoc, &bp_loc)) != STATUS_OK) {
			error("rm_get_data(RM_BPLoc): %s", bgl_err_str(rc));
			continue;
		}
		if ((rc = rm_get_data(my_bp, RM_BPState, &bp_state)) 
				!= STATUS_OK) {
			error("rm_get_data(RM_BPState): %s", bgl_err_str(rc));
			continue;
		}
		if  (bp_state == RM_BP_DOWN) 		/* already down */
			continue;

		snprintf(bgl_down_node, sizeof(bgl_down_node), "bgl%d%d%d",
			bp_loc.X, bp_loc.Y, bp_loc.Z);
#ifdef USE_BGL_FILES
		if ((rc1 = rm_set_data(my_bp, RM_BPState, RM_BP_DOWN)) 
				!= STATUS_OK)
			error("switch for node %s is bad, could not set down, "
				"rm_set_data(RM_BPState): %s",
				bgl_down_node, bgl_err_str(rc1));
		else
			info("switch for node %s is bad, set down", 
				bgl_down_node);
#else
		info("FIXME: switch for node %s is bad, set down", bgl_down_node);
#endif
	}
}
#endif

#ifdef HAVE_BGL_FILES
/* Convert base partition state value to a string */
static char *_convert_bp_state(rm_BP_state_t state)
{
	switch(state) { 
	case RM_BP_UP:
		return "RM_BP_UP";
		break;
	case RM_BP_DOWN:
		return "RM_BP_DOWN";
		break;
	case RM_BP_NAV:
		return "RM_BP_NAV";
	}
	return "BP_STATE_UNIDENTIFIED!";
}
#endif

/* Test for nodes that are DOWN in BlueGene database, 
 * if so DRAIN them in SLURM */ 
extern void test_down_nodes(void)
{
#ifdef HAVE_BGL_FILES
	int bp_num, i, rc;
	rm_BP_t *my_bp;
	rm_BP_state_t bp_state;
	rm_location_t bp_loc;
	char down_node_list[BUFSIZE];
	char bgl_down_node[128];
	static rm_BGL_t *bgl = NULL;

	if (bgl)
		return;
	if ((rc = rm_get_BGL(&bgl)) != STATUS_OK) {
		error("rm_get_BGL(): %s", bgl_err_str(rc));
		return;
	}

	debug("Running test_down_nodes");
	down_node_list[0] = '\0';
	if ((rc = rm_get_data(bgl, RM_BPNum, &bp_num)) != STATUS_OK) {
		error("rm_get_data(RM_BPNum): %s", bgl_err_str(rc));
		bp_num = 0;
	}
	for (i=0; i<bp_num; i++) {
		if (i) {
			if ((rc = rm_get_data(bgl, RM_NextBP, &my_bp)) 
					!= STATUS_OK) {
				error("rm_get_data(RM_NextBP): %s", 
					bgl_err_str(rc));
				continue;
			}
		} else {
			if ((rc = rm_get_data(bgl, RM_FirstBP, &my_bp)) 
					!= STATUS_OK) {
				error("rm_get_data(RM_FirstBP): %s", 
					bgl_err_str(rc));
				continue;
			}
		}

		if ((rc = rm_get_data(my_bp, RM_BPState, &bp_state)) 
				!= STATUS_OK) {
			error("rm_get_data(RM_BPState): %s", bgl_err_str(rc));
			continue;
		}
		if  (bp_state != RM_BP_DOWN)
			continue;
		if ((rc = rm_get_data(my_bp, RM_BPLoc, &bp_loc)) 
				!= STATUS_OK) {
			error("rm_get_data(RM_BPLoc): %s", bgl_err_str(rc));
			continue;
		}

		snprintf(bgl_down_node, sizeof(bgl_down_node), "bgl%d%d%d", 
			bp_loc.X, bp_loc.Y, bp_loc.Z);
		debug("_test_down_nodes: %s in state %s", 
			bgl_down_node, _convert_bp_state(RM_BPState));
		if ((strlen(down_node_list) + strlen(bgl_down_node) + 2) 
				< BUFSIZE) {
			if (down_node_list[0] != '\0')
				strcat(down_node_list,",");
			strcat(down_node_list, bgl_down_node);
		} else
			error("down_node_list overflow");
	}
	slurm_rm_free_BGL(bgl);

	if (down_node_list[0]) {
		char reason[128];
		time_t now = time(NULL);
		struct tm * time_ptr = localtime(&now);
		strftime(reason, sizeof(reason), 
			"bluegene_select: CMCS state DOWN [SLURM@%b %d %H:%M]", 
			time_ptr);
		slurm_drain_nodes(down_node_list, reason);
	}
#endif
}

/* Test for switches that are DOWN in BlueGene database, 
 * when found DRAIN them in SLURM and configure their base partition DOWN */
void test_down_switches(void)
{
#ifdef HAVE_BGL_FILES
	int switch_num, i, rc;
	rm_switch_t *my_switch;
	rm_bp_id_t bp_id;
	rm_switch_state_t switch_state;
	static rm_BGL_t *bgl = NULL;

	if (bgl)
		return;

	if ((rc = rm_get_BGL(&bgl)) != STATUS_OK) {
		error("rm_get_BGL(): %s", bgl_err_str(rc));
		return;
	}

	debug2("Running test_down_switches");
	if ((rc = rm_get_data(bgl, RM_SwitchNum, &switch_num)) != STATUS_OK) {
		error("rm_get_data(RM_SwitchNum): %s", bgl_err_str(rc));
		switch_num = 0;
	}
	for (i=0; i<switch_num; i++) {
		if (i) {
			if ((rc = rm_get_data(bgl, RM_NextSwitch, &my_switch))
					!= STATUS_OK) {
				error("rm_get_data(RM_NextSwitch): %s", 
					bgl_err_str(rc));
				continue;
			}
		} else {
			if ((rc = rm_get_data(bgl, RM_FirstSwitch, &my_switch))
					!= STATUS_OK) {
				error("rm_get_data(RM_FirstSwitch): %s",
					bgl_err_str(rc));
				continue;
			}
		}

		if ((rc = rm_get_data(my_switch, RM_SwitchState, 
				&switch_state)) != STATUS_OK) {
			error("rm_get_data(RM_SwitchState): %s",
				bgl_err_str(rc));
			continue;
		}
		if (switch_state != RM_SWITCH_DOWN)
			continue;
		if ((rc = rm_get_data(my_switch, RM_SwitchBPID, &bp_id)) 
				!= STATUS_OK) {
			error("rm_get_data(RM_SwitchBPID): %s",
				bgl_err_str(rc));
			continue;
		}
		_configure_node_down(bp_id, bgl);
	}
	
	slurm_rm_free_BGL(bgl);
#endif
}
