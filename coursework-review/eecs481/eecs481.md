# Overview

- core course topics
    - mesurement and risk
        - process, scheduling and information
    - quality assurance
        - code review, testing and analysis
    - software defects
        - reporting and localizing
    - software design
        - requirements, patterns and maintainability
    - productivity at scale
        - people, teams, interviews, synthesis and brains
- What is software engineering?
    - the majority of industrial software engineering is not writing code
    - the dominant activities in software engineering are comprehension and maintenance
    - software engineering
        - is a human process
        - deals with large scales
        - requires strategic thinking
        - constrained by reality

# Measurement and Risk

- __software development process__ devides software development into distinct phases to improve design, product and project management
    - processes can increase efficiency, but are often implemented poorly
    - __waterfall model__
        1. system and software requirements: captured in a document
        2. analysis: resulting in models, schema, and business rules
        3. design: resulting in software architecture
        4. coding: the development, proving and integration of software
        5. testing: the systematic discovery and debugging of defects
        6. operations: the installation, migration, support, and maintenance of complete systems
    - __spiral development model__ focuses on the construction of an increasingly-complete series of prototypes while accounting for risk
- __effort estimation__ is based on historical information, it is complicated by __uncertainty__, which stems from __risk__, which can be __managed__
- defects are more expensive to repair if they are created in earlier phases
- __software metrics__ are widely used in industry to support decision-making
    - metrics are often inadequately supported and thus lack validity, they should be used carefully
- __measurement__ is a fundamental activity but is influenced by human biases
    - it is easy to __misinterpret__ data or fucus on what is easy to __measure__
    - metrics can __incentivize__ perverse behavior


# Quality Assurance

- __quality assurance__ maintains desired product properties through process choices
- __testing__ involves running the program and inspecting its results or behavior
    - it is the dominant approach to software quality assurance
    - there are numerous methods of testing
        - __unit testing__ and __integration testing__
        - __regression testing__: when you fix a bug, run tests again to make sure fixing the bug does not introduce unintended breaks
- when __unit testing__ depends on some external API, which is difficult, expensive or unavailable, __mocking__ uses simple replacement functionality for this external API
    - mock objects are simulated objects that mimic behavior of real objects in controlled ways
    - in __unit testing__, mocking uses a mock object to test the behavior of some other object
        ```python
        import requests

        def make_api_call(url):
            response = requests.get(url)
            return response.json()

        def process_data():
            data = make_api_call('https://api.example.com/data')
            # Assume data is a list of numbers
            return sum(data)
        ```

        ```python
        import unittest
        from unittest.mock import patch
        from your_module import process_data  # Assuming the above functions are in 'your_module.py'

        class TestProcessData(unittest.TestCase):
            
            @patch('your_module.make_api_call')
            def test_process_data(self, mock_make_api_call):
                # Configure the mock to return a specific response
                mock_make_api_call.return_value = [1, 2, 3, 4, 5]
                
                result = process_data()
                
                # Verify that the function produces the expected output
                self.assertEqual(result, 15)
        ```