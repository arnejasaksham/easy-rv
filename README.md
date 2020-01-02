# easy-rv

## About
This project provides an easy-to-use implementation of _Runtime Verification_, based on formal semantics.

This project presents a more generalised any-type monitoring system, which can be used with any C project. 
_easy-rv_ was ported from [easy-rte](https://github.com/PRETgroup/easy-rte), which implemented a similar semantics for runtime enforcement.

## What is Runtime Verification?

Runtime Verification is a type of Run-time Assurance (RA) which focuses on formal semantics for monitoring the behaviour of systems.
As a system executes, the monitor observes according to formal policies. 
In each instant (a.k.a. tick/reaction), the monitor will output one of `{currently true, currently false, true, false}`, to indicate the status of the system.
This project aims to join the growing chorus calling for the use of Runtime Assurance mechanisms for use within the real-world cyber-physical systems domain. Using Runtime Verification, we can monitor the behaviour of _unpredictable_ and _untrustworthy_ processes, ensuring that they any violation of desired policies is detected.

## How does Runtime Verification work?

Runtime Verification works by placing a special observer execution module _between_ your plant and your controller.

```
+-------+                +------------+               
|       | ---inputs--->  |            |
| Plant |                | Controller |
|       | <--outputs---  |            |
+-------+                +------------+
```
becomes
```
             Policies
                \/
            +---------+
            |         |
            | Monitor |
            |         |
            +---------+
                 |
                 |
+-------+       /|       +------------+               
|       | ---inputs--->  |            |
| Plant |        /       | Controller |
|       | <--outputs---  |            |
+-------+                +------------+
```

In our case, we can compile our policies to either *C* or *Verilog* code. 

The C monitors are designed to be composed with your software in a system such as a microcontroller or an Arduino.

However, software monitors cannot by their nature monitor the behaviour of the hardware that they run upon. 
So, in this case you may want to build your monitor to Verilog, and then compose your microcontroller with custom hardware (such as on an FPGA or ASIC) to ensure system correctness.

## Build instructions

Download and install the latest version of [Go](https://golang.org/doc/install).

Then, download this repository, and run `make` or `make default`, which will generate the tools. 

* The ab5 example can be generated using `make c_mon PROJECT=ab5`.
* You can also generate Verilog monitors by using `make verilog_mon PROJECT=ab5`, for example. The Verilog monitors are a little trickier to use, and require I/O connections to be provided to them. They do not embed a function call to the controller inside.
* If you are interested in using the model checkers:
  * Obtain CBMC (C model checker) by running `sudo apt install cbmc`. Website here: https://www.cprover.org/cbmc/
  * Obtain EBMC (Verilog model checker) by going to website https://www.cprover.org/ebmc/

## A note on Easy-rv language

Easy-rv is based on Structured Text (ST) operators and syntax. When making guards, ensure that you adhere to the following operators:

| Operator       |     Code    |
| -------------- | ----------- |
| Assignment     |  `:=`       |
| Equality       |  `=`        |
| Inequality     |  `<>`       |
| Addition       |  `+`        |
| Subtraction    |  `-`        |
| Multiplication |  `*`        |
| Division       |  `/`        |
| Not            | `!` or NOT  |
| And            | `&&` or AND |
| Or             | `\|\|` or OR  |
| Brackets       | `(` and `)` |

## Example of Use (Pizza)

Let us consider the case of a frozen pizza.
It has the following lifetime:

![Pizza Lifespan Image](/example/pizza/Easy-RV-Pizza.png)

We can represent this state machine with the following _erv_ specification:

```


```

## Example of Use (AB5)

Imagine a function which inputs boolean `A` and outputs boolean `B`. 
In _easy-rv_, we present this function with the following _erv_ syntax:
```
monitor ab5;
interface of ab5 {
	bool A;  //in this case, A goes from PLANT to CONTROLLER
	bool B; //in this case, B goes from CONTROLLER to PLANT
}
```

This is equivalent to the following C code, which is autogenerated, so that the function ab5Function_run can be provided by the user:
```c
//IO of the monitor ab5Function
typedef struct {
	bool A;
} io_ab5_t;


void ab5_run(io_ab5_t io);
```

Let's now give our function the following I/O properties:
1. A and B alternate starting with an A. 
2. B should be true within 5 ticks after an occurance of A.
3. A and B can only happen simultaneously in the first instant, or after any B. This indicates the end of a run.

We can present this as the following _easy-rv_ policy format:
```
policy AB5 of ab5 {
	internals {
		dtimer_t v;
	}

	states {

		//first state is initial, and represents "We're waiting for an A"
		s0 accepting {
			//if we receive neither A nor B, do nothing														
			-> s0 on (!A and !B): v := 0;

			//if we receive an A only, head to state s1							
			-> s1 on (A and !B): v := 0;
			
			//if we receive a !A and B then VIOLATION							
			-> violation on (!A and B);

			//if we receive both A and B then done
			-> done on (A and B);	
		}

		//s1 is "we're waiting for a B, and it needs to get here within 5 ticks"
		s1 rejecting {
			//if we receive nothing, and we aren't over-time, then we do nothing														
			-> s1 on (!A and !B and v < 5);	

			//if we receive a B only, head to state s0					
			-> s0 on (!A and B);

			//if we go overtime, or we receive another A, then VIOLATION	
			-> violation on ((v >= 5) or (A and B) or (A and !B));
		}

		done accepting trap;

		violation rejecting trap;
	}
}
```

As can be seen, this can be thought of as a simple mealy finite state machine, which provides the rules for correct operation.

We may now compile this system to executable code.

With _easy-rv_, this process is completed automatically in two steps. Firstly, we convert the _erv_ file into an equivalent policy XML file (which makes it easier to understand, and allows portability between tools).
* `./easy-rv-parser -i example/ab5 -o example/ab5`

Then, we convert this policy XML file into executable code, which is written in C. 
* `./easy-rv-c -i example/ab5 -o example/ab5`

Now, we can provide a `main.c` file which has our controller and plant interface code in it, and then compile the project together.

This entire example is provided in the `/example/ab5` folder of this repository, including an example top level file, and can be built from the root directory using `make c_mon PROJECT=ab5`.

To compile it together with the example main file, run `make c_mon c_build PROJECT=ab5`

