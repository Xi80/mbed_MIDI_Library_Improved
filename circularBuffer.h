#ifndef H_CIRCULARBUFFER
#define H_CIRCULARBUFFER
#include "mbed.h"
#include "string.h"
namespace lunaLib{
	template<typename T, uint16_t Capacity>
	class CircularBuffer{
	private:
		T buffer[Capacity];
		const uint16_t m_mask;
		uint16_t m_head, m_tail;
		uint16_t m_size;
		uint16_t m_capacity;

		inline void MovePointer(uint16_t &pointer, const int16_t &step){
			pointer = (pointer + step) & m_mask;
		}
		inline uint16_t emptySize() const{
			return Capacity - m_size;
		}

	public:
		volatile bool isLocked;
		explicit CircularBuffer() :m_mask(Capacity - 1), m_head(0), m_tail(0), m_size(0), m_capacity(Capacity), isLocked(false){
			if ((Capacity & (Capacity - 1)) != 0) while (true);
		}

		virtual ~CircularBuffer(){
		}

		void push_back(const T &value){
			if (isLocked) while (true);
			isLocked = true;
			buffer[m_tail] = value;
			MovePointer(m_tail, 1);
			if (m_size == Capacity)
				MovePointer(m_head, 1);
			else m_size++;
			isLocked = false;
		}
		T pull(){
			if (isLocked) while (true);
			isLocked = true;
			const T res = buffer[m_head];
			MovePointer(m_head, 1);
			m_size--;
			isLocked = false;
			return res;
		}

		uint16_t size() const{
			return m_size;
		}

		void clear(){
			if (isLocked) while (true)
				;
			isLocked = true;

			m_head = 0;
			m_tail = 0;
			m_size = 0;

			isLocked = false;
		}

		T &operator[ ](const uint16_t &n){
			return buffer[(m_head + n) & m_mask];
		}
	};

};
#endif

