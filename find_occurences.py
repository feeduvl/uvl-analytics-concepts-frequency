import subprocess
import json


def find_occurences(content, result, program_name):
    '''
    Call the program with a list of result concepts and note the number of occurences in each text
    :param content:
    :param result:
    :param program_name:
    :return:
    '''
    result.topics.text_ids = []
    result.topics.text_occurences = []

    for doc in content["dataset"]["documents"]:
        args = ['./lib/'+program_name,
                "find_occurences",
                doc["text"]]

        args += result.topics.concepts

        output_ = subprocess.run(args, capture_output=True)
        outs_ = output_.stdout.decode("utf-8", errors="replace")  #  should never be an error

        j = json.loads(outs_)

        result.topics.text_ids.append(doc["id"])
        result.topics.text_occurences.append(j.occurences)

    return result
