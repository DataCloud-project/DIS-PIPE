import glob
import subprocess
import re
import sys

from os import path

# to be kept equal to equivalent constants in ResultPerspective.java
FD_DIR = path.join(path.dirname(path.abspath(__file__)), 'fast-downward')
SRC_DIR = path.join(FD_DIR, 'PDDLFiles')
DEST_DIR = path.join(FD_DIR, 'plans_found')
DEST_FILE = path.join(DEST_DIR, 'alignment_')

ALIGNMENT_FILE_POS = 5
DOMAIN_FILE_POS = 6
PROBLEM_FILE_POS = DOMAIN_FILE_POS + 1
PLANNER_INPUT_EXT = '.pddl'
DOMAIN_FILE_PATTERN = 'domain*'
PROBLEM_FILE_PATTERN = 'problem'
INTEGER_NUMBER_REGEX = '\d+'
DECIMAL_NUMBER_REGEX = '\d+(,\d{3})*(\.\d+)*'
TOTAL_TIME_ENTRY_PREFIX = 'Total time: '
SEARCH_TIME_ENTRY_PREFIX = '; searchtime = '
TIME_CONVERSION_COEFF = 1000


if __name__ == '__main__':

    # the argument list has to match the following structure:
    #
    # ['< planner_manager_path >',  # to be discarded
    # 'python',
    # '.../fast-downward/fast-downward.py',
    # '--build',
    # '<build_id>',
    # '--plan-file',
    # '< alignment_file_placeholder >',
    # '< domain_file_placeholder >',
    # '< problem_file_placeholder >',
    # '--heuristic',
    # '< chosen_heuristic >',
    # '--search',
    # '< chosen_strategy >']

    planner_args = sys.argv[1:]

    for domain in glob.glob(path.join(SRC_DIR, DOMAIN_FILE_PATTERN)):
        # extract trace number
        basename = path.basename(domain)
        trace_number = re.search(INTEGER_NUMBER_REGEX, basename).group(0)

        # get correct file names for problem and output
        problem = path.join(SRC_DIR, PROBLEM_FILE_PATTERN + trace_number + PLANNER_INPUT_EXT)
        alignment = DEST_FILE + trace_number

        # exec planner (redirecting output)
        planner_args[ALIGNMENT_FILE_POS] = alignment
        planner_args[DOMAIN_FILE_POS] = domain
        planner_args[PROBLEM_FILE_POS] = problem
        process = subprocess.Popen(planner_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # get planner search time from process std out
        out, err = process.communicate()

        if err:
            sys.stderr.write(err+'\n')

        trace_alignment_time = re.search(TOTAL_TIME_ENTRY_PREFIX + DECIMAL_NUMBER_REGEX, out).group(0)
        trace_alignment_time = re.search(DECIMAL_NUMBER_REGEX, trace_alignment_time).group(0)
        trace_alignment_time_ms = float(trace_alignment_time) * TIME_CONVERSION_COEFF

        # append runtime to output file
        with open(alignment, 'a') as alignment_file:
            alignment_file.write(SEARCH_TIME_ENTRY_PREFIX + str(trace_alignment_time_ms))
