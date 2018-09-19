#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "Queue.h"

TEST_CASE("An empty queue", "[queue]") {
	Queue<int> queue;

	REQUIRE(queue.empty());
	REQUIRE(queue.size() == 0u);

	SECTION("inserting an element makes the map not empty") {
		queue.push(2);

		REQUIRE(!queue.empty());
	}

	SECTION("inserting an element increases the size") {
		queue.push(4);

		REQUIRE(queue.size() == 1u);
	}

	SECTION("pop on empty queue does nothing") {
		queue.push(6);
		queue.pop();

		REQUIRE(queue.size() == 0);
		REQUIRE(queue.empty());
	}
}

TEST_CASE("Create a queue list with multiple elements", "[queue]") {
	Queue<int> queue;
	queue.push(2);
	queue.push(4);
	queue.push(6);
	queue.push(8);
	queue.push(10);

	static auto init_values = std::vector<int>{ 2, 4, 6, 8, 10 };

	REQUIRE(queue.size() == init_values.size());
	REQUIRE(!queue.empty());
	REQUIRE(std::distance(queue.begin(), queue.end()) == init_values.size());
	REQUIRE(std::equal(queue.begin(), queue.end(), init_values.begin()));

	SECTION("Can find elements with std::find") {
		auto found = std::find(std::begin(queue), std::end(queue), 4);

		REQUIRE(found != std::end(queue));
		REQUIRE(*found == 4);
	}

	SECTION("pop removes last element") {
		queue.pop();

		REQUIRE(queue.front() == 8);
		REQUIRE(queue.size() == 4);
	}

	SECTION("copy construction") {
		auto second_list = queue;

		REQUIRE(queue.size() == init_values.size());
		REQUIRE(std::equal(queue.begin(), queue.end(), init_values.begin()));
		REQUIRE(second_list.size() == queue.size());
		REQUIRE(std::equal(second_list.begin(), second_list.end(), queue.begin()));
	}

	SECTION("copy assignment") {
		auto second_list = Queue<int>{};

		second_list = queue;

		REQUIRE(queue.size() == init_values.size());
		REQUIRE(std::equal(queue.begin(), queue.end(), init_values.begin()));
		REQUIRE(second_list.size() == queue.size());
		REQUIRE(std::equal(second_list.begin(), second_list.end(), queue.begin()));
	}

	SECTION("move construction leaves original list in empty state") {
		auto second_list = Queue<int>{ std::move(queue) };

		REQUIRE(queue.empty());
		REQUIRE(second_list.size() == init_values.size());
		REQUIRE(std::equal(second_list.begin(), second_list.end(), init_values.begin()));
	}

	std::cin.get();
}
