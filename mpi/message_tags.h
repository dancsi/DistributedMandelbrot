#pragma once

enum Tag {
	none,
	job_request,
	job_reply_new_job,
	job_reply_no_jobs,
	job_results_header, 
	job_results_data
};