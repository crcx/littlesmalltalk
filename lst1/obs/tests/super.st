Class One
[
        test
                ^ 1
|       result1
                ^ self test
]

Class Two :One
[
        test
                ^ 2
]

Class Three :Two
[
        result2
                ^ self result1
|       result3
                ^ super test
]

Class Four :Three
[
        test
                ^ 4
]

Class Main
| example1 example2 example3 example4 |
[
        main
                example1 <- One new.
                example2 <- Two new.
                example3 <- Three new.
                example4 <- Four new.
                example1 test print.
                example1 result1 print.
                example2 test print.
                example2 result1 print.
                example3 test print.
                example4 result1 print.
                example3 result2 print.
                example4 result2 print.
                example3 result3 print.
                example4 result3 print
]
