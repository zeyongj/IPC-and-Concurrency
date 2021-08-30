# IPC and Concurrency

## Introduction
- SFU CMPT 300: Operating Systems I, Individual Project: IPC and Concurrency via C.
- This application is used to launch conversations between two users. The files of list.h and list.o are provided by the instructor.

## Required package
- pthread.

## Execution
- `make`
- ./s-talk [my UDP port] [remote machine name] [remote machine's UDP port]

## EXAMPLE of usage

Assuming that USER1's local UDP port is 6060, and USER2's local UDP is 6061, then

- USER1 input:

    `./s-talk 6060 USER2 6061`
    
- USER2 input:

    `./s-talk 6061 USER1 6060`
    
Then the chat session can begin.

## Termination

An s-talk session is terminated when either user enters the complete line of text which is just one `!` and presses enter. 

EXAMPLE of termination as follows:

Assuming that USER1's local UDP port is 6060, and USER2's local UDP is 6061, then

- USER1 input: `!`
    
- Or: USER2 input: `!`
    
Then the screen shows "total terminate the session" and the chat session stops.

## License

This work is licensed under [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0) (or any later version). 

`SPDX-License-Identifier: Apache-2.0-or-later`

## Disclaimer

**This repository is *ONLY* for backup. Students should *NEVER* use this repository to finish their works, *IN ANY WAY*.**

It is expected that within this course, the highest standards of academic integrity will be maintained, in
keeping with SFU’s Policy S10.01, `Code of Academic Integrity and Good Conduct`.

In this class, collaboration is encouraged for in-class exercises and the team components of the assignments, as well
as task preparation for group discussions. However, individual work should be completed by the person
who submits it. Any work that is independent work of the submitter should be clearly cited to make its
source clear. All referenced work in reports and presentations must be appropriately cited, to include
websites, as well as figures and graphs in presentations. If there are any questions whatsoever, feel free
to contact the course instructor about any possible grey areas.

Some examples of unacceptable behaviour:
- Handing in assignments/exercises that are not 100% your own work (in design, implementation,
wording, etc.), without a clear/visible citation of the source.
- Using another student's work as a template or reference for completing your own work.
- Using any unpermitted resources during an exam.
- Looking at, or attempting to look at, another student's answer during an exam.
- Submitting work that has been submitted before, for any course at any institution.

All instances of academic dishonesty will be dealt with severely and according to SFU policy. This means
that Student Services will be notified, and they will record the dishonesty in the student's file. Students
are strongly encouraged to review SFU’s Code of Academic Integrity and Good Conduct (S10.01) available
online at: http://www.sfu.ca/policies/gazette/student/s10-01.html.

## Author

Zeyong, JIN

August 23, 2021
