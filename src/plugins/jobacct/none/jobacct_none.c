
/*****************************************************************************\
 *  jobacct_none.c - NO-OP slurm job completion logging plugin.
 *****************************************************************************
 *
 *  Copyright (C) 2005 Hewlett-Packard Development Company, L.P.
 *  Written by Andy Riebs, <andy.riebs@hp.com>.
 *  UCRL-CODE-217948.
 *  
 *  This file is part of SLURM, a resource management program.
 *  For details, see <http://www.llnl.gov/linux/slurm/>.
 *  
 *  SLURM is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  In addition, as a special exception, the copyright holders give permission 
 *  to link the code of portions of this program with the OpenSSL library under
 *  certain conditions as described in each individual source file, and 
 *  distribute linked combinations including the two. You must obey the GNU 
 *  General Public License in all respects for all of the code used other than 
 *  OpenSSL. If you modify file(s) with this exception, you may extend this 
 *  exception to your version of the file(s), but you are not obligated to do 
 *  so. If you do not wish to do so, delete this exception statement from your
 *  version.  If you delete this exception statement from all source files in 
 *  the program, then also delete it here.
 *  
 *  SLURM is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with SLURM; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 *  This file is patterned after jobcomp_none.c, written by Morris Jette and
 *  Copyright (C) 2002 The Regents of the University of California.
\*****************************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#if HAVE_STDINT_H
#  include <stdint.h>
#endif
#if HAVE_INTTYPES_H
#  include <inttypes.h>
#endif

#include <stdio.h>
#include <slurm/slurm_errno.h>

#include "src/slurmctld/slurmctld.h"
#include "src/slurmd/slurmd/slurmd.h"
#include "src/common/slurm_jobacct.h"

/*
 * These variables are required by the generic plugin interface.  If they
 * are not found in the plugin, the plugin loader will ignore it.
 *
 * plugin_name - a string giving a human-readable description of the
 * plugin.  There is no maximum length, but the symbol must refer to
 * a valid string.
 *
 * plugin_type - a string suggesting the type of the plugin or its
 * applicability to a particular form of data or method of data handling.
 * If the low-level plugin API is used, the contents of this string are
 * unimportant and may be anything.  SLURM uses the higher-level plugin
 * interface which requires this string to be of the form
 *
 *	<application>/<method>
 *
 * where <application> is a description of the intended application of
 * the plugin (e.g., "jobacct" for SLURM job completion logging) and <method>
 * is a description of how this plugin satisfies that application.  SLURM will
 * only load job completion logging plugins if the plugin_type string has a 
 * prefix of "jobacct/".
 *
 * plugin_version - an unsigned 32-bit integer giving the version number
 * of the plugin.  If major and minor revisions are desired, the major
 * version number may be multiplied by a suitable magnitude constant such
 * as 100 or 1000.  Various SLURM versions will likely require a certain
 * minimum versions for their plugins as the job accounting API 
 * matures.
 */
const char plugin_name[] = "Job accounting NOT_INVOKED plugin";
const char plugin_type[] = "jobacct/none";
const uint32_t plugin_version = 100;

/*
 * init() is called when the plugin is loaded, before any other functions
 * are called.  Put global initialization here.
 */
extern int init ( void )
{
	verbose("%s loaded", plugin_name);
	return SLURM_SUCCESS;
}

extern int fini ( void )
{
	return SLURM_SUCCESS;
}

/*
 * The following routines are called by slurmctld
 */

/*
 * The following routines are called by slurmd
 */
int jobacct_p_init_struct(struct jobacctinfo *jobacct, 
			  jobacct_id_t *jobacct_id)
{
	return SLURM_SUCCESS;
}

struct jobacctinfo *jobacct_p_alloc(jobacct_id_t *jobacct_id)
{
	return NULL;
}

void jobacct_p_free(struct jobacctinfo *jobacct)
{
	return;
}

int jobacct_p_setinfo(struct jobacctinfo *jobacct, 
		      enum jobacct_data_type type, void *data)
{
	return SLURM_SUCCESS;
	
}

int jobacct_p_getinfo(struct jobacctinfo *jobacct, 
		      enum jobacct_data_type type, void *data)
{
	return SLURM_SUCCESS;
}

void jobacct_p_aggregate(struct jobacctinfo *dest, struct jobacctinfo *from)
{
	return;
}

void jobacct_p_2_sacct(sacct_t *sacct, struct jobacctinfo *jobacct)
{
	return;
}

void jobacct_p_pack(struct jobacctinfo *jobacct, Buf buffer)
{
	return;
}

int jobacct_p_unpack(struct jobacctinfo **jobacct, Buf buffer)
{
	return SLURM_SUCCESS;
}


int jobacct_p_init_slurmctld(char *job_acct_log)
{
	return SLURM_SUCCESS;
}

int jobacct_p_fini_slurmctld()
{
	return SLURM_SUCCESS;
}

int jobacct_p_job_start_slurmctld(struct job_record *job_ptr)
{
	return SLURM_SUCCESS;
}

int jobacct_p_job_complete_slurmctld(struct job_record *job_ptr) 
{
	return  SLURM_SUCCESS;
}

int jobacct_p_step_start_slurmctld(struct step_record *step)
{
	return SLURM_SUCCESS;	
}

int jobacct_p_step_complete_slurmctld(struct step_record *step)
{
	return SLURM_SUCCESS;	
}

int jobacct_p_suspend_slurmctld(struct job_record *job_ptr)
{
	return SLURM_SUCCESS;
}

int jobacct_p_startpoll(int frequency)
{
	info("jobacct NONE plugin loaded");
	debug3("slurmd_jobacct_init() called");
	
	return SLURM_SUCCESS;
}

int jobacct_p_endpoll()
{
	return SLURM_SUCCESS;
}

int jobacct_p_set_proctrack_container_id(uint32_t id)
{
	return SLURM_SUCCESS;
}

int jobacct_p_add_task(pid_t pid, jobacct_id_t *jobacct_id)
{
	return SLURM_SUCCESS;
}

struct jobacctinfo *jobacct_p_stat_task(pid_t pid)
{
	return NULL;
}

struct jobacctinfo *jobacct_p_remove_task(pid_t pid)
{
	return NULL;
}

void jobacct_p_suspend_poll()
{
	return;
}

void jobacct_p_resume_poll()
{
	return;
}
