1) Producer-consumer problem

Propose a first implementation that implements the behaviour above without considering synchronisation issues.

class CircularBuffer:
    def __init__(self, max_size):
        self.max_size = max_size
        self.buffer = [None] * max_size
        self.start = 0
        self.end = 0
        self.size = 0

    def is_full(self):
        return self.size == self.max_size

    def is_empty(self):
        return self.size == 0

    def append(self, item):
        if self.is_full():
            raise BufferError("Buffer is full")
        self.buffer[self.end] = item
        self.end = (self.end + 1) % self.max_size
        self.size += 1

    def remove(self):
        if self.is_empty():
            raise BufferError("Buffer is empty")
        item = self.buffer[self.start]
        self.buffer[self.start] = None
        self.start = (self.start + 1) % self.max_size
        self.size -= 1
        return item


cb = CircularBuffer(5)
cb.append(1)
cb.append(2)
print(cb.remove())  
print(cb.remove())  
							







Extend this implementation by considering that two producers (resp. two consumers) can
store (resp. remove) data at the same time in the buffer. Use synchronisation mechanisms
and provide their initialisation.

import threading

class CircularBuffer:
    def __init__(self, max_size):
        self.max_size = max_size
        self.buffer = [None] * max_size
        self.start = 0
        self.end = 0
        self.size = 0
        self.mutex = threading.Semaphore(1)  # Binary semaphore for mutual exclusion
        self.empty_slots = threading.Semaphore(max_size)  # Counting semaphore for empty slots
        self.full_slots = threading.Semaphore(0)  # Counting semaphore for full slots

    def is_full(self):
        return self.size == self.max_size

    def is_empty(self):
        return self.size == 0

    def append(self, item):
        self.empty_slots.acquire()  # Wait if there are no empty slots
        self.mutex.acquire()  # Acquire the mutex to ensure mutual exclusion
        try:
            self.buffer[self.end] = item
            self.end = (self.end + 1) % self.max_size
            self.size += 1
        finally:
            self.mutex.release()  # Release the mutex
            self.full_slots.release()  # Signal that there's a new full slot

    def remove(self):
        self.full_slots.acquire()  # Wait if there are no full slots
        self.mutex.acquire()  # Acquire the mutex to ensure mutual exclusion
        try:
            item = self.buffer[self.start]
            self.buffer[self.start] = None
            self.start = (self.start + 1) % self.max_size
            self.size -= 1
        finally:
            self.mutex.release()  # Release the mutex
            self.empty_slots.release()  # Signal that there's a new empty slot
        return item

def producer(buffer, items):
    for item in items:
        buffer.append(item)
        print(f"Produced: {item}")

def consumer(buffer, num_items):
    for _ in range(num_items):
        item = buffer.remove()
        print(f"Consumed: {item}")

# Example usage
buffer = CircularBuffer(5)

producer_thread1 = threading.Thread(target=producer, args=(buffer, [1, 2, 3]))
producer_thread2 = threading.Thread(target=producer, args=(buffer, [4, 5, 6]))
consumer_thread1 = threading.Thread(target=consumer, args=(buffer, 3))
consumer_thread2 = threading.Thread(target=consumer, args=(buffer, 3))

producer_thread1.start()
producer_thread2.start()
consumer_thread1.start()
consumer_thread2.start()

producer_thread1.join()
producer_thread2.join()
consumer_thread1.join()
consumer_thread2.join()


Extend this implementation by considering that a producer has to block when the buffer is
full.
Extend this implementation by considering that a consumer has to block when the buffer is
empty.

import threading
import time

class CircularBuffer:
    def __init__(self, max_size):
        self.max_size = max_size
        self.buffer = [None] * max_size
        self.start = 0
        self.end = 0
        self.size = 0
        self.empty_slots = threading.Semaphore(max_size)  # Counting semaphore for empty slots
        self.full_slots = threading.Semaphore(0)  # Counting semaphore for full slots
        self.mutex = threading.Semaphore(1)  # Semaphore for mutual exclusion

    def append(self, item):
        self.empty_slots.acquire()  # Wait if there are no empty slots
        self.mutex.acquire()  # Enter critical section
        try:
            self.buffer[self.end] = item
            self.end = (self.end + 1) % self.max_size
            self.size += 1
        finally:
            self.mutex.release()  # Exit critical section
            self.full_slots.release()  # Signal that there's a new full slot

    def remove(self):
        self.full_slots.acquire()  # Wait if there are no full slots
        self.mutex.acquire()  # Enter critical section
        try:
            item = self.buffer[self.start]
            self.buffer[self.start] = None
            self.start = (self.start + 1) % self.max_size
            self.size -= 1
        finally:
            self.mutex.release()  # Exit critical section
            self.empty_slots.release()  # Signal that there's a new empty slot
        return item

def produce(buffer, item):
    while True:
        try:
            buffer.append(item)
            print(f"Produced: {item}")
            time.sleep(1)  # Simulate production time
        except BufferError:
            time.sleep(0.1)

def consume(buffer):
    while True:
        try:
            item = buffer.remove()
            print(f"Consumed: {item}")
            time.sleep(1)  # Simulate consumption time
        except BufferError:
            time.sleep(0.1)
    
cb = CircularBuffer(5)

prod1 = threading.Thread(target=produce, args=(cb, 1))
prod2 = threading.Thread(target=produce, args=(cb, 2))
consumer1 = threading.Thread(target=consume, args=(cb,))
consumer2 = threading.Thread(target=consume, args=(cb,))

prod1.start()
prod2.start()
consumer1.start()
consumer2.start()

prod1.join()
prod2.join()
consumer1.join()
consumer2.join()



2)  Rendex-vouz :

import threading

# Initialize semaphores with initial value 0
sem1 = threading.Semaphore(0)
sem2 = threading.Semaphore(0)

def process1():
    print("Process 1: Reached rendez-vous point.")
    sem1.release()  # Signal arrival to process2
    sem2.acquire()  # Wait for process2's signal
    print("Process 1: Both processes have arrived, proceeding.")

def process2():
    print("Process 2: Reached rendez-vous point.")
    sem2.release()  # Signal arrival to process1
    sem1.acquire()  # Wait for process1's signal
    print("Process 2: Both processes have arrived, proceeding.")

# Create and start threads for both processes
thread1 = threading.Thread(target=process1)
thread2 = threading.Thread(target=process2)

thread1.start()
thread2.start()

thread1.join()
thread2.join()

3) Barber Shop

a)
import threading
import time
import random

# Number of seats in the waiting room
N = 5

# Semaphores
waiting_room = threading.Semaphore(N)  # Semaphore for available seats
barber_chair = threading.Semaphore(1)  # Semaphore for the barber chair
barber_ready = threading.Semaphore(0)  # Semaphore to signal the barber is ready
customer_ready = threading.Semaphore(0)  # Semaphore to signal a customer is ready

def customer(id):
    print(f"Customer {id} arrives at the shop.")
    if waiting_room.acquire(blocking=False):
        print(f"Customer {id} sits in the waiting room.")
        customer_ready.release()  # Signal that a customer is ready
        barber_ready.acquire()  # Wait for the barber to be ready
        waiting_room.release()  # Leave the waiting room
        print(f"Customer {id} is getting a haircut.")
        barber_chair.acquire()  # Sit in the barber chair
        time.sleep(random.randint(1, 3))  # Simulate haircut time
        barber_chair.release()  # Leave the barber chair
        print(f"Customer {id} leaves the shop.")
    else:
        print(f"Customer {id} finds no seat available and leaves.")

def barber():
    while True:
        print("Barber is waiting for a customer.")
        customer_ready.acquire()  # Wait for a customer to be ready
        barber_ready.release()  # Signal the barber is ready
        print("Barber is cutting hair.")
        time.sleep(random.randint(1, 3))  # Simulate haircut time

# Create and start the barber thread
barber_thread = threading.Thread(target=barber)
barber_thread.start()

# Create and start customer threads
for i in range(10):
    threading.Thread(target=customer, args=(i,)).start()
    time.sleep(random.randint(1, 2))  # Simulate random arrival times

b)

import threading
import time
import random

# Number of seats in the waiting room
N = 5

# Semaphores
waiting_room = threading.Semaphore(N)  # Semaphore for available seats
barber_chair = threading.Semaphore(1)  # Semaphore for the barber chair
barber_ready = threading.Semaphore(0)  # Semaphore to signal the barber is ready
customer_ready = threading.Semaphore(0)  # Semaphore to signal a customer is ready
mutex = threading.Semaphore(1)  # Mutual exclusion for waiting room access

def customer(id):
    print(f"Customer {id} arrives at the shop.")
    mutex.acquire()
    if waiting_room._value > 0:
        waiting_room.acquire()
        mutex.release()
        print(f"Customer {id} sits in the waiting room.")
        customer_ready.release()  # Signal that a customer is ready
        barber_ready.acquire()  # Wait for the barber to be ready
        print(f"Customer {id} is getting a haircut.")
        barber_chair.acquire()  # Sit in the barber chair
        mutex.acquire()
        waiting_room.release()  # Leave the waiting room
        mutex.release()
        time.sleep(random.randint(1, 3))  # Simulate haircut time
        barber_chair.release()  # Leave the barber chair
        print(f"Customer {id} leaves the shop.")
    else:
        mutex.release()
        print(f"Customer {id} finds no seat available and leaves.")

def barber():
    while True:
        print("Barber is waiting for a customer.")
        customer_ready.acquire()  # Wait for a customer to be ready
        barber_ready.release()  # Signal the barber is ready
        print("Barber is cutting hair.")
        time.sleep(random.randint(1, 3))  # Simulate haircut time

# Create and start the barber thread
barber_thread = threading.Thread(target=barber)
barber_thread.start()

# Create and start customer threads
for i in range(10):
    threading.Thread(target=customer, args=(i,)).start()
    time.sleep(random.randint(1, 2))  # Simulate random arrival times

5)
a)
import threading
import time
import random

class BusStation:
	def __init__(self, seats):
    	self.seats = threading.Semaphore(seats)
    	self.queue_lock = threading.Lock()
    	self.board_condition = threading.Condition(self.queue_lock)
    	self.queue = []

	def passenger(self, id):
    	print(f"Passenger {id} arrives at the station.")
    	with self.queue_lock:
        	self.queue.append(id)
    	with self.board_condition:
        	self.board_condition.notify()

	def bus_driver(self):
    	while True:
        	with self.board_condition:
            	self.board_condition.wait_for(lambda: len(self.queue) > 0)
            	print("Bus arrives.")
            	while self.seats._value > 0 and len(self.queue) > 0:
                	with self.queue_lock:
                    	passenger_id = self.queue.pop(0)
                	self.seats.acquire()
                	print(f"Passenger {passenger_id} boards the bus.")
                	time.sleep(random.uniform(0.5, 1.5))  # Simulate boarding time
                	print(f"Passenger {passenger_id} has boarded.")
            	print("Bus departs.")
            	break

N = 5  # Number of seats on the bus
num_passengers = 10  # Number of passengers arriving

station = BusStation(N)

driver_thread = threading.Thread(target=station.bus_driver)
driver_thread.start()

for i in range(num_passengers):
	threading.Thread(target=station.passenger, args=(i,)).start()
	time.sleep(random.uniform(0.5, 1.0))  # Simulate random arrival times

driver_thread.join()

b)
import threading
import time
import random

class BusStation:
	def __init__(self, seats):
    	self.seats = threading.Semaphore(seats)
    	self.queue_lock = threading.Lock()
    	self.board_condition = threading.Condition(self.queue_lock)
    	self.queue = []

	def passenger(self, id):
    	print(f"Passenger {id} arrives at the station.")
    	with self.queue_lock:
        	self.queue.append(id)
    	with self.board_condition:
        	self.board_condition.notify_all()

	def bus_driver(self):
    	while True:
        	with self.board_condition:
            	# Wait until there are passengers in the queue
            	self.board_condition.wait_for(lambda: len(self.queue) > 0)
            	print("Bus arrives.")

            	while self.seats._value > 0 and len(self.queue) > 0:
                	with self.queue_lock:
                    	passenger_id = self.queue.pop(0)
                	self.seats.acquire()
                	print(f"Passenger {passenger_id} boards the bus.")
                	# Simulate boarding time
                	time.sleep(random.uniform(0.5, 1.5))
                	print(f"Passenger {passenger_id} has boarded.")

                	# If there are no more seats or no more passengers, driver leaves
                	if self.seats._value == 0 or len(self.queue) == 0:
                    	print("Bus departs.")
                    	break
            	else:
                	# Notify all that boarding is complete and driver leaves
                	with self.queue_lock:
                    	self.board_condition.notify_all()
                    	break

N = 5 
num_passengers = 10 

station = BusStation(N)

driver_thread = threading.Thread(target=station.bus_driver)
driver_thread.start()

for i in range(num_passengers):
	threading.Thread(target=station.passenger, args=(i,)).start()
	time.sleep(random.uniform(0.5, 1.0))  # Simulate random arrival times

driver_thread.join()
