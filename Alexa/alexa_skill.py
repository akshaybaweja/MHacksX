from __future__ import print_function
import json
import random
# --------------- Helpers that build all of the responses ----------------------
def build_speechlet_response(title, output, reprompt_text, should_end_session):
    return {
        'outputSpeech': {
            'type': 'PlainText',
            'text': output
        },
        'card': {
            'type': 'Simple',
            'title': title,
            'content': output
        },
        'reprompt': {
            'outputSpeech': {
                'type': 'PlainText',
                'text': reprompt_text
            }
        },
        'shouldEndSession': should_end_session
    }

def build_response(session_attributes, speechlet_response):
    return {
        'version': '1.0',
        'sessionAttributes': session_attributes,
        'response': speechlet_response
    }

# --------------- Functions that control the skill's behavior ------------------

def get_welcome_response():
    card_title = "Welcome to Vibra Driver"
    session_attributes = {}
    should_end_session = False
    speech_output = "Welcome to Vibra Driver MHacks X Alexa Skill."
    reprompt_text=speech_output
    return build_response(session_attributes, build_speechlet_response(
        card_title, speech_output, reprompt_text, should_end_session))

# def get_property(intent, session):
#     card_title = "Element Property"
#     session_attributes = {}
#     should_end_session = True
#     speech_output=""
#     if "element_name" in intent['slots']:
#         givenElement = intent['slots']['element_name']
#         if(givenElement.has_key('value')):
#             givenElement = givenElement['value']
#             data = get_element_details(givenElement)
#             if data != None:
#                 if "Property" in intent['slots']:
#                     property_value = intent['slots']['Property']['value']
#                     data_property = get_element_property(data,property_value.lower())
#                     speech_output = "The "+property_value+" of "+givenElement+" is "+str(data_property)
#                     card_title = "Element Trivia - Property"
#                 else:
#                     speech_output = "No property found"
#            else:
#                speech_output = "Element data not found"
#         else:
#             speech_output = "Please specify an element name and try again, "\
#                 "Say help for more info."
#     else:
#         speech_output = "No Element Name Found"

#     reprompt_text=speech_output
#     return build_response(session_attributes, build_speechlet_response(
#         card_title, speech_output, reprompt_text, should_end_session))

def getInfo(intent,session):
    card_title = "Drive Information"
    session_attributes = {}
    should_end_session = True
    speech_output = ""
    randoms = [0,1,2,3,4,5]
    if random.choice(randoms) <= 2:
        speech_output = "You were fine today. Have a great day"
    elif random.choice(randoms) == 3:
        speech_output = "You had a bumpy ride today. You are sleep deprived. Do you want to set alarm for me to wake you up"
    else :
        speech_output = "You drove pretty bad today and were very sleep deprived. You should see a doctor. Do you want me to book an appointment?"
    reprompt_text = speech_output
    return build_response(session_attributes, build_speechlet_response(
      card_title, speech_output, reprompt_text, should_end_session))
    

def handle_session_end_request():
    card_title = "Thank you for using Vibra Drive. Drive Safe."
    speech_output = "Thank you for using Vibra Drive. Drive Safe."
    should_end_session = True
    return build_response({}, build_speechlet_response(
        card_title, speech_output, None, should_end_session))

def get_help():
    card_title = "Help"
    speech_output = "You can ask me how did your driving go today. How many times you dozed off is also what I can answer."
    should_end_session = False
    return build_response({}, build_speechlet_response(
        card_title, speech_output, speech_output, should_end_session))

# --------------- Events ------------------

def on_session_started(session_started_request, session):
    """ Called when the session starts """

    print("on_session_started requestId=" + session_started_request['requestId']
          + ", sessionId=" + session['sessionId'])


def on_launch(launch_request, session):
    """ Called when the user launches the skill without specifying what they
    want
    """

    print("on_launch requestId=" + launch_request['requestId'] +
          ", sessionId=" + session['sessionId'])
    # Dispatch to your skill's launch
    return get_welcome_response()


def on_intent(intent_request, session):
    """ Called when the user specifies an intent for this skill """

    print("on_intent requestId=" + intent_request['requestId'] +
          ", sessionId=" + session['sessionId'])

    intent = intent_request['intent']
    intent_name = intent_request['intent']['name']
    print(str(intent))
    # Dispatch to your skill's intent handlers
    if intent_name == "DriveInfo":
        return getInfo(intent, session)
    elif intent_name == "AMAZON.HelpIntent":
        return get_help(intent,session)
    elif intent_name == "AMAZON.CancelIntent" or intent_name == "AMAZON.StopIntent":
        return handle_session_end_request()
    else:
        raise ValueError("Invalid intent")


def on_session_ended(session_ended_request, session):
    """ Called when the user ends the session.
    Is not called when the skill returns should_end_session=true
    """
    print("on_session_ended requestId=" + session_ended_request['requestId'] +
          ", sessionId=" + session['sessionId'])
    # add cleanup logic here


# --------------- Main handler ------------------

def lambda_handler(event, context):
    
    
    
    #print("event.session.application.applicationId=" +
    #       event['session']['application']['applicationId'])

    if event['session']['new']:
        event['session']['attributes'] = {}
        on_session_started({'requestId': event['request']['requestId']},
                           event['session'])
    if event['request']['type'] == "LaunchRequest":
        return on_launch(event['request'], event['session'])
    elif event['request']['type'] == "IntentRequest":
        return on_intent(event['request'], event['session'])
    elif event['request']['type'] == "SessionEndedRequest":
        return on_session_ended(event['request'], event['session'])