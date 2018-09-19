#ifndef Queue_h
#define Queue_h

#include <iterator>
#include <memory>

template <class T>
class Queue {
private:
	struct Node {
		T data;
		std::unique_ptr<Node> next = nullptr;

		template<typename... Args, typename = std::enable_if_t<std::is_constructible<T, Args&&...>::value>>
		explicit Node(std::unique_ptr<Node>&& next, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value)
			: data{ std::forward<Args>(args)... }, next{ std::move(next) } {}

		// disable if noncopyable<T> for cleaner error msgs
		explicit Node(const T& x, std::unique_ptr<Node>&& p = nullptr)
			: data(x)
			, next(std::move(p)) {}

		// disable if nonmovable<T> for cleaner error msgs
		explicit Node(T&& x, std::unique_ptr<Node>&& p = nullptr)
			: data(std::move(x))
			, next(std::move(p)) {}
	};
	std::unique_ptr<Node> first = nullptr;
	Node* last = nullptr;

	void do_unchecked_pop() {
		first = std::move(first->next);
	}

	// Use for debugging purposes and for overloading the << operator
	void show(std::ostream &str) const;

public:
	// Constructors
	Queue() = default;														  // empty constructor
	Queue(Queue const& source);												  // copy constructor

	// Rule of 5
	Queue(Queue&& move) noexcept;                                             // move constuctor
	Queue& operator=(Queue&& move) noexcept;                                  // move assignment operator
	~Queue();                                                                 // destructor

	// Overload operators
	Queue& operator=(Queue const& rhs);
	friend std::ostream& operator<<(std::ostream& str, Queue<T> const& data) {
		data.show(str);
		return str;
	}

	// Create an iterator class
	class iterator;
	iterator begin();
	iterator end();
	iterator before_begin();

	// Create const iterator class
	class const_iterator;
	const_iterator cbegin() const;
	const_iterator cend() const;
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator before_begin() const;
	const_iterator cbefore_begin() const;

	// Member functions
	template<typename... Args>
	iterator emplace(const_iterator pos, Args&&... args);

	bool empty() const { return first == nullptr; }
	int size() const;
	T& front() const;
	T& back() const;
	void push(const T& theData);
	void push(T&& theData);
	void pop();
	void swap(Queue& other) noexcept;

};

template <class T>
class Queue<T>::iterator {
	Node* node = nullptr;
	bool before_begin = false;

public:
	friend class Queue<T>;

	using iterator_category = std::forward_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T * ;
	using reference = T & ;

	operator const_iterator() const noexcept { return const_iterator{ node }; }
	iterator(Node* node = nullptr, bool before = false) : node{ node }, before_begin{ before } {}

	bool operator!=(iterator other) const noexcept;
	bool operator==(iterator other) const noexcept;

	T& operator*() const { return node->data; }
	T& operator->() const { return &node->data; }

	iterator& operator++();
	iterator operator++(int);
};

template <class T>
class Queue<T>::const_iterator {
	Node* node = nullptr;
	bool before_begin = false;

public:
	friend class Queue<T>;

	using iterator_category = std::forward_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = const T *;
	using reference = const T &;

	const_iterator() = default;
	const_iterator(Node* node, bool before = false) : node{ node }, before_begin{ before } {}


	bool operator!=(const_iterator other) const noexcept;
	bool operator==(const_iterator other) const noexcept;

	const T& operator*() const { return node->data; }
	const T& operator->() const { return &node->data; }

	const_iterator& operator++();
	const_iterator operator++(int);
};

template <class T>
Queue<T>::Queue(Queue<T> const& source) {
	try {
		for (auto loop = source.first.get(); loop != nullptr; loop = loop->next.get())
			push(loop->data);
	}
	catch (...) {
		while (first != nullptr)
			do_unchecked_pop();
		throw;
	}
}


template <class T>
Queue<T>::Queue(Queue&& move) noexcept {
	move.swap(*this);
}

template <class T>
Queue<T>& Queue<T>::operator=(Queue<T> &&move) noexcept {
	move.swap(*this);
	return *this;
}

template <class T>
Queue<T>::~Queue() {
	while (first) {
		do_unchecked_pop();
	}
}

template <class T>
Queue<T>& Queue<T>::operator=(Queue const& rhs) {
	Queue copy(rhs);
	swap(copy);
	return *this;
}

template <class T>
template <typename... Args>
typename Queue<T>::iterator Queue<T>::emplace(const_iterator pos, Args&&... args) {
	if (pos.before_begin) {
		emplace_front(std::forward<Args>(args)...);
		return begin();
	}

	if (pos.node->next) {
		pos.node->next = std::make_unique<Node>(std::move(pos.node->next), std::forward<Args>(args)...);  // Creating a new node that has the old next pointer with the new value and assign it to the next pointer of the current node
		return { pos.node->next.get() };
	}
}

template <class T>
int Queue<T>::size() const {
	int size = 0;
	for (auto current = first.get(); current != nullptr; current = current->next.get()) 
		size++;

	return size;
}

template <class T>
T& Queue<T>::front() const {
	if (!empty()) {
		return first->data;
	}
	else {
		throw std::out_of_range("The queue is empty!");
	}
}

template <class T>
T& Queue<T>::back() const {
	if (!empty()) {
		return last->data;
	}
	else {
		throw std::out_of_range("The queue is empty!");
	}
}

template <class T>
void Queue<T>::push(const T& theData) {
	std::unique_ptr<Node> newNode = std::make_unique<Node>(theData);

	if (!first) {
		first = std::move(newNode);
		last = first.get();
	}
	else {
		last->next = std::move(newNode);
		last = last->next.get();
	}
}

template <class T>
void Queue<T>::push(T&& theData) {
	std::unique_ptr<Node> newNode = std::make_unique<Node>(std::move(theData));

	if (!first) {
		first = std::move(newNode);
		last = first.get();
	}
	else {
		last->next = std::move(newNode);
		last = last->next.get();
	}
}

template <class T>
void Queue<T>::pop() {
	if (empty()) {
		throw std::out_of_range("The queue is empty!");
	}
	do_unchecked_pop();
}

template <class T>
void Queue<T>::swap(Queue<T>& other) noexcept {
	using std::swap;
	swap(first, other.first);
	swap(last, other.last);
}

template <class T>
void Queue<T>::show(std::ostream &str) const {
	for (auto loop = first.get(); loop != nullptr; loop = loop->next.get()) {
		str << loop->data << "\t";
	}
	str << "\n";
}

// Iterator Implementation /////////////////////////////////////////////////////////////////////////
template <class T>
typename Queue<T>::iterator& Queue<T>::iterator::operator++() {
	if (before_begin) before_begin = false;
	else node = node->next.get();

	return *this;
}

template <class T>
typename Queue<T>::iterator Queue<T>::iterator::operator++(int) {
	auto copy = *this;
	++*this;
	return copy;
}

template<typename T>
bool Queue<T>::iterator::operator==(iterator other) const noexcept {
	return node == other.node && before_begin == other.before_begin;
}

template<typename T>
bool Queue<T>::iterator::operator!=(iterator other) const noexcept {
	return !(*this == other);
}

template<class T>
typename Queue<T>::iterator Queue<T>::begin() {
	return first.get();
}

template<class T>
typename Queue<T>::iterator Queue<T>::end() {
	return {};
}

template <class T>
typename Queue<T>::iterator Queue<T>::before_begin() {
	return { first.get(), true };
}

// Const Iterator Implementation ////////////////////////////////////////////////////////////////////
template <class T>
typename Queue<T>::const_iterator& Queue<T>::const_iterator::operator++() {
	if (before_begin) before_begin = false;
	else node = node->next.get();

	return *this;
}

template<typename T>
typename Queue<T>::const_iterator Queue<T>::const_iterator::operator++(int) {
	auto copy = *this;
	++*this;
	return copy;
}

template<typename T>
bool Queue<T>::const_iterator::operator==(const_iterator other) const noexcept {
	return node == other.node && before_begin == other.before_begin;
}

template<typename T>
bool Queue<T>::const_iterator::operator!=(const_iterator other) const noexcept {
	return !(*this == other);
}


template <class T>
typename Queue<T>::const_iterator Queue<T>::begin() const {
	return first.get();
}

template <class T>
typename Queue<T>::const_iterator Queue<T>::end() const {
	return {};
}

template <class T>
typename Queue<T>::const_iterator Queue<T>::cbegin() const {
	return begin();
}

template <class T>
typename Queue<T>::const_iterator Queue<T>::cend() const {
	return end();
}

template <class T>
typename Queue<T>::const_iterator Queue<T>::before_begin() const {
	return { first.get(), true };
}

template <class T>
typename Queue<T>::const_iterator Queue<T>::cbefore_begin() const {
	return before_begin();
}
#endif /* Queue_h */