import subprocess
import json


def find_occurences(content, result, program_name, logger):
    '''
    Call the program with a list of result concepts and note the number of occurences in each text
    :param content:
    :param result:
    :param program_name:
    :param logger: 
    :return:
    '''

    text_ids, text_occurences = [], []
    result["topics"]["text_ids"] = text_ids
    result["topics"]["text_occurences"] = text_occurences

    for doc in content["dataset"]["documents"]:
        args = ['./lib/'+program_name,
                "find_occurences",
                doc["text"],
                content["params"]["term_length"]]

        args += result["topics"]["concepts"]

        output_ = subprocess.run(args, capture_output=True)
        outs_ = output_.stdout.decode("utf-8", errors="replace")  #  should never be an error

        logger.debug(outs_)
        j = json.loads(outs_)

        text_ids.append(doc["id"])
        text_occurences.append(j["occurences"])

    return result
