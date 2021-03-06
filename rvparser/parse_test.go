package rvparser

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"reflect"
	"testing"

	"github.com/PRETgroup/easy-rv/rvdef"
)

type ParseTest struct {
	Name   string
	Input  string
	Output []rvdef.Monitor //if applicable
	Err    error           //if applicable
}

var basicTests = []ParseTest{
	{
		Name: "simple typo 1",
		Input: `monitor testBlock;
				interface of asdasd {}`,
		Err: ErrUndefinedFunction,
	},
	{
		Name:  "simple typo 2",
		Input: `dadasdasd`,
		Err:   ErrUnexpectedValue,
	},
	{
		Name: "simple typo 3",
		Input: `monitor testBlock1, , testBlock3;
				interface of testBlock2 {}`,
		Err: ErrUnexpectedValue,
	},
	{
		Name: "simple typo 4",
		Input: `monitor testBlock1, testBlock2;
				interface of testBlock2;`,
		Err: ErrUnexpectedValue,
	},
	{
		Name: "simple typo 5",
		Input: `monitor testBlock1, testBlock2;
				interface of testBlock2 {`,
		Err: ErrUnexpectedEOF,
	},
	{
		Name: "simple typo 6",
		Input: `monitor testBlock1, testBlock2;
				interface of testBlock2 {}
				policy AB of testBlock2 {`,
		Err: ErrUnexpectedEOF,
	},
	{
		Name: "simple typo 7",
		Input: `monitor testBlock1;
				policy AB of asdasd {}`,
		Err: ErrUndefinedFunction,
	},
	{
		Name: "simple typo 8",
		Input: `monitor testBlock1, testBlock2;
				architecture testBlock2 {}`,
		Err: ErrUnexpectedValue,
	},
	{
		Name: "missing word 1",
		Input: `monitor testBlock1;
				interface testBlock1 {}`,
		Err: ErrUnexpectedValue,
	},
	{
		Name: "empty interface 1",
		Input: `monitor testBlock;
				interface of testBlock {}`,
		Output: []rvdef.Monitor{
			rvdef.Monitor{
				Name:          "testBlock",
				InterfaceList: []rvdef.Variable(nil),
				Policies:      []rvdef.Policy(nil),
			},
		},
		Err: nil,
	},
	{
		Name: "empty interfaces 1",
		Input: `monitor testBlock1, testBlock2, testBlock3;
				interface of testBlock2 {}`,
		Output: []rvdef.Monitor{
			rvdef.Monitor{
				Name:          "testBlock1",
				InterfaceList: []rvdef.Variable(nil),
				Policies:      []rvdef.Policy(nil),
			},
			rvdef.Monitor{
				Name:          "testBlock2",
				InterfaceList: []rvdef.Variable(nil),
				Policies:      []rvdef.Policy(nil),
			},
			rvdef.Monitor{
				Name:          "testBlock3",
				InterfaceList: []rvdef.Variable(nil),
				Policies:      []rvdef.Policy(nil),
			},
		},
		Err: nil,
	},
}

func runParseTests(t *testing.T, pTests []ParseTest) {
	for i, test := range pTests {
		out, err := ParseString(fmt.Sprintf("Test[%d]", i), test.Input)
		if err != nil && test.Err == nil {
			t.Errorf("Test[%d](%s): Error '%s' occurred when it shouldn't have", i, test.Name, err.Error())
		} else if err == nil && test.Err != nil {
			t.Errorf("Test[%d](%s): Error didn't occur and it should have been '%s'", i, test.Name, test.Err.Error())
		} else if err != nil && test.Err != nil {
			if err.Err.Error() != test.Err.Error() {
				t.Errorf("Test[%d](%s): Error codes don't match (it was '%s', should have been '%s')", i, test.Name, err.Error(), test.Err.Error())
			}
		} else if err == nil && test.Err == nil {
			if !reflect.DeepEqual(out, test.Output) {
				t.Errorf("Test[%d](%s): Outputs don't match!", i, test.Name)

				bytes, _ := json.MarshalIndent(test.Output, "", "\t")
				//fmt.Printf("\n\nDesired:\n%s", bytes)
				ioutil.WriteFile("test_desired.out.json", bytes, 0644)

				bytes, _ = json.MarshalIndent(out, "", "\t")
				ioutil.WriteFile("test_actual.out.json", bytes, 0644)

				goString := fmt.Sprintf("%#v", out)
				ioutil.WriteFile("test_actual.out.govar", []byte(goString), 0644)

			}

		}

	}
}

func TestParseBasics(t *testing.T) {
	runParseTests(t, basicTests)
}
