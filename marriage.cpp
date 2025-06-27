#include <iostream>
#include <vector>
#include <string>
#include "test.h"

class MarriageEvent;

class MarriageAction {
public:
    virtual void perform(MarriageEvent &event) = 0;
    virtual ~MarriageAction() = default;
};

class MarriageEvent {
    std::string couple_name;
    std::string event_type;
    bool is_approved;
    std::vector<MarriageAction*> actions;

public:
    MarriageEvent(const std::string &name, const std::string &type, bool approved,
                 const std::vector<MarriageAction*> &acts)
        : couple_name(name), event_type(type),
          is_approved(approved), actions(acts) {}

    std::string getCoupleName() const { return couple_name; }
    std::string getEventType() const { return event_type; }
    bool isApproved() const { return is_approved; }

    void setApproved(bool approved) { is_approved = approved; }

    void process() {
        for (auto *action : actions) {
            action->perform(*this);
        }
    }

    ~MarriageEvent() {
        for (auto *action : actions) {
            delete action;
        }
    }
};

class ApproveMarriage : public MarriageAction {
public:
    void perform(MarriageEvent &event) override {
        event.setApproved(true);
    }
};

class RejectMarriage : public MarriageAction {
public:
    void perform(MarriageEvent &event) override {
        event.setApproved(false);
    }
};

class ConditionalApproval : public MarriageAction {
    MarriageAction* action;
    int min_age;
public:
    ConditionalApproval(MarriageAction* act, int age) : action(act), min_age(age) {}
    void perform(MarriageEvent &event) override {
        if (event.getEventType() == "Adult" || min_age >= 18) {
            action->perform(event);
        }
    }
    ~ConditionalApproval() { delete action; }
};

class MarriageRegistry {
    std::vector<MarriageEvent*> events;
public:
    void addEvent(MarriageEvent *event) { events.push_back(event); }

    void processAll() {
        for (auto *event : events) {
            event->process();
        }
    }

    ~MarriageRegistry() {
        for (auto *event : events) {
            delete event;
        }
    }
};

TEST(MarriageTest, ApprovalWorkflow) {
    MarriageRegistry registry;
    std::vector<MarriageAction*> actions = { new ApproveMarriage() };
    MarriageEvent* wedding = new MarriageEvent("John & Jane", "Wedding", false, actions);
    registry.addEvent(wedding);
    registry.processAll();
    ASSERT_TRUE(wedding->isApproved());
    return true;
}

TEST(MarriageTest, RejectionWorkflow) {
    MarriageRegistry registry;
    std::vector<MarriageAction*> actions = { new RejectMarriage() };
    MarriageEvent* wedding = new MarriageEvent("Mike & Sarah", "Wedding", true, actions);
    registry.addEvent(wedding);
    registry.processAll();
    ASSERT_TRUE(!wedding->isApproved());
    return true;
}

TEST(MarriageTest, MultipleEventsProcessing) {
    MarriageRegistry registry;
    std::vector<MarriageAction*> actions1 = { new ApproveMarriage() };
    std::vector<MarriageAction*> actions2 = { new RejectMarriage() };

    MarriageEvent* event1 = new MarriageEvent("Tom & Lisa", "Wedding", false, actions1);
    MarriageEvent* event2 = new MarriageEvent("Alex & Emily", "Wedding", true, actions2);

    registry.addEvent(event1);
    registry.addEvent(event2);
    registry.processAll();

    ASSERT_TRUE(event1->isApproved());
    ASSERT_TRUE(!event2->isApproved());
    return true;
}

TEST(MarriageTest, ConditionalAdultApproval) {
    MarriageRegistry registry;
    std::vector<MarriageAction*> actions = { new ConditionalApproval(new ApproveMarriage(), 20) };
    MarriageEvent* adultWedding = new MarriageEvent("David & Sophia", "Adult", false, actions);
    registry.addEvent(adultWedding);
    registry.processAll();
    ASSERT_TRUE(adultWedding->isApproved());
    return true;
}

TEST(MarriageTest, ConditionalMinorRejection) {
    MarriageRegistry registry;
    std::vector<MarriageAction*> actions = { new ConditionalApproval(new ApproveMarriage(), 16) };
    MarriageEvent* minorWedding = new MarriageEvent("Kevin & Mia", "Minor", false, actions);
    registry.addEvent(minorWedding);
    registry.processAll();
    ASSERT_TRUE(!minorWedding->isApproved());
    return true;
}

int main() {
    RUN_TEST(MarriageTest, ApprovalWorkflow);
    RUN_TEST(MarriageTest, RejectionWorkflow);
    RUN_TEST(MarriageTest, MultipleEventsProcessing);
    RUN_TEST(MarriageTest, ConditionalAdultApproval);
    RUN_TEST(MarriageTest, ConditionalMinorRejection);
    return 0;
}
