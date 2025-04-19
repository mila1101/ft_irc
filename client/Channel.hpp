#pragma once

#include <string>
#include <set>
#include <map>

class Channel {
	private:
		std::string name_;
		std::string topic_;
		std::set<int> members_;
		std::set<int> operators_;
		std::map<int, bool> invited_;

		// modes for the channel
		bool inviteOnly_ = false;
		bool topicRestriction_ = false;
		std::string password_;
		int userLimit_ = 10; // if we want unlimited we can just put -1

	public:
		Channel() {}
		Channel(const std::string &name) : name_(name), topic_("") {}

		const std::string &getName() const { return name_; }

		const std::string &getTopic() const { return topic_; }
		void setTopic(const std::string &topic) { topic_ = topic; }

		bool isMember(int fd) const { return members_.find(fd) != members_.end(); }
		void addMember(int fd) { members_.insert(fd); }
		void removeMember(int fd) {
			members_.erase(fd);
			operators_.erase(fd);
		}
		const std::set<int> &getMembers() const { return members_; }
		bool isEmpty() const { return members_.empty(); }

		bool isOperator(int fd) const { return operators_.find(fd) != operators_.end(); }
		void addOperator(int fd) { operators_.insert(fd); }
		void removeOperator(int fd) { operators_.erase(fd); }
		const std::set<int> &getOperators() const { return operators_; }

		void invite(int fd) { invited_[fd] = true; }
		bool isInvited(int fd) const {
			std::map<int, bool>::const_iterator it = invited_.find(fd);
			return it != invited_.end() && it->second;
		}
		void clearInvite(int fd) { invited_.erase(fd); }

		bool isInviteOnly() const { return inviteOnly_; }
		void setInviteOnly(bool value) { inviteOnly_ = value; }

		bool isTopicRestriction() const { return topicRestriction_; }
		void setTopicRestriction_(bool value) { topicRestriction_ = value; }

		const std::string &getPassword() const { return password_; }
		void setPassword(const std::string &pass) { password_ = pass; }
		bool hasPassword() const {return !password_.empty(); }

		int getUserLimit() const { return userLimit_; }
		void setUserLimit(int limit) { userLimit_ = limit; }
		bool hasUserLimit() const { return userLimit_ >= 0; }
		bool isFull() const { return userLimit_ >= 0 && static_cast<int>(members_.size()) >= userLimit_; }

};
