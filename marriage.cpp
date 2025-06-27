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

int main() {
    RUN_TEST(MarriageTest, ApprovalWorkflow);
    return 0;
}