#pragma once

#ifdef GRIDENG_EXPORTS
#define GRIDENG_API __declspec(dllexport)
#else 
#define GRIDENG_API __declspec(dllimport)
#endif

#include <random>

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 750.0f };

constexpr float up_tile_y{ -100.0f };
constexpr float down_tile_y{ 850.0f };

constexpr float left_tile_x{ -100.0f };
constexpr float right_tile_x{ scr_width + 100.0f };

constexpr int GRID_MAX_COLS{ 16 };
constexpr int GRID_MAX_ROWS{ 20 };

constexpr int BAG_OK = 4000;
constexpr int BAG_BAD_PTR = 4001;
constexpr int BAG_BAD_INDEX = 4002;
constexpr int BAG_UNKNOWN_ERR = 4003;

constexpr unsigned char no_collision{ 0b00000000 };

constexpr unsigned char up_flag{ 0b00000001 };
constexpr unsigned char down_flag{ 0b00000010 };
constexpr unsigned char left_flag{ 0b00000100 };
constexpr unsigned char right_flag{ 0b00001000 };

constexpr unsigned char up_left_flag{ 0b00000101 };
constexpr unsigned char up_right_flag{ 0b00000110 };
constexpr unsigned char down_left_flag{ 0b00001001 };
constexpr unsigned char down_right_flag{ 0b00001010 };


enum class dirs { up_left = 0, up = 1, up_right = 2, right = 3, down_right = 4, down = 5, down_left = 6, left = 7, stop = 8 };
enum class tiles { dirt = 0, forest = 1, lava = 2, path = 3, water = 4 };
enum class pigs { fly = 0, tough = 1, runner = 2, freak = 3, hero = 4 };
enum class food { bacon = 0, cheese = 1, fish = 2, pizza = 3, rotten = 4 };
enum class obstacles { rock = 0, fence = 1 };

struct GRIDENG_API FPOINT
{
	float x{ 0 };
	float y{ 0 };
};
struct GRIDENG_API FRECT
{
	float left{ 0 };
	float up{ 0 };
	float right{ 0 };
	float down{ 0 };
};
struct GRIDENG_API TILE
{
	tiles type{ tiles::dirt };
	
	FRECT dims{};
	
	float move_reduct{ 0 };
};

namespace dll
{
	template<typename T> class BAG
	{
	private:
		T* m_ptr{ nullptr };
		size_t max_size{ 0 };
		size_t next_pos{ 0 };
		bool valid = false;

	public:
		BAG() :max_size{ 1 }, m_ptr{ reinterpret_cast<T>(calloc(1, sizeof(T))) } {};
		BAG(size_t capacity) :max_size{ capacity }, m_ptr{ reinterpret_cast<T>(calloc(capacity, sizeof(T))) } {};
		BAG(BAG& other)
		{
			max_size = other.max_size;
			m_ptr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			for (size_t count = 0; count < other.next_pos; ++count)m_ptr[count] = other.m_ptr[count];
			next_pos = other.next_pos;
			valid = other.valid;
		}
		BAG(BAG&& other)
		{
			m_ptr = other.m_ptr;
			max_size = other.max_size;
			next_pos = other.next_pos;
			valid = other.valid;
			other.m_ptr = nullptr;
		}
		~BAG()
		{
			free(m_ptr);
			m_ptr = nullptr;
		}

		int push_back(T element)
		{
			if (!m_ptr)
			{
				valid = false;
				return BAG_BAD_PTR;
			}
			else
			{
				if (next_pos + 1 <= max_size)
				{
					m_ptr[next_pos] = element;
					++next_pos;
					valid = true;
					return BAG_OK;
				}
				else
				{
					++max_size;
					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, max_size * sizeof(T)));
					if (!m_ptr)return BAG_BAD_PTR;
					else
					{
						m_ptr[next_pos] = element;
						++next_pos;
						valid = true;
						return BAG_OK;
					}
				}
			}

			return BAG_UNKNOWN_ERR;
		}
		int push_back(T* element)
		{
			if (!m_ptr)
			{
				valid = false;
				return BAG_BAD_PTR;
			}
			else
			{
				if (next_pos + 1 <= max_size)
				{
					m_ptr[next_pos] = (*element);
					++next_pos;
					valid = true;
					return BAG_OK;
				}
				else
				{
					++max_size;
					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, max_size * sizeof(T)));
					if (!m_ptr)return BAG_BAD_PTR;
					else
					{
						m_ptr[next_pos] = *element;
						++next_pos;
						valid = true;
						return BAG_OK;
					}
				}
			}

			return BAG_UNKNOWN_ERR;
		}

		int push_front(T element)
		{
			if (!m_ptr)
			{
				valid = false;
				return BAG_BAD_PTR;
			}
			else
			{
				if (next_pos + 1 <= max_size)
				{
					for (size_t count = next_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
					m_ptr[0] = element;
					++next_pos;
					valid = true;
					return BAG_OK;
				}
				else
				{
					++max_size;
					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, max_size * sizeof(T)));
					if (!m_ptr)
					{
						valid = false;
						return BAG_BAD_PTR;
					}
					else
					{
						for (size_t count = next_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
						m_ptr[0] = element;
						++next_pos;
						valid = true;
						return BAG_OK;
					}
				}
			}

			return BAG_UNKNOWN_ERR;
		}
		int push_front(T* element)
		{
			if (!m_ptr)
			{
				valid = false;
				return BAG_BAD_PTR;
			}
			else
			{
				if (next_pos + 1 <= max_size)
				{
					for (size_t count = next_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
					m_ptr[0] = *element;
					++next_pos;
					valid = true;
					return BAG_OK;
				}
				else
				{
					++max_size;
					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, max_size * sizeof(T)));
					if (!m_ptr)
					{
						valid = false;
						return BAG_BAD_PTR;
					}
					else
					{
						for (size_t count = next_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
						m_ptr[0] = *element;
						++next_pos;
						valid = true;
						return BAG_OK;
					}
				}
			}

			return BAG_UNKNOWN_ERR;
		}

		int erase(size_t index)
		{
			if (index < 0 || index >= next_pos)return BAG_BAD_INDEX;
			if (!m_ptr)return BAG_BAD_PTR;
			else
			{
				for (size_t count = index; count < next_pos - 1; count++)m_ptr[count] = m_ptr[count + 1];
				--next_pos;
				return BAG_OK;
			}

			return BAG_UNKNOWN_ERR;
		}

		BAG& operator = (BAG& other)
		{
			BAG dummy();
			
			if (m_ptr != other.m_ptr)
			{
				free(m_ptr);

				max_size = other.max_size;
				m_ptr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
				next_pos = other.next_pos;
				valid = other.valid;
				if (!m_ptr)return dummy;
				else
				{
					if (next_pos > 0)for (size_t count = 0; count < next_pos; ++count)m_ptr[count] = other.m_ptr[count];
					return (*this);
				}
			}

			return dummy;
		}
		BAG& operator=(BAG&& other)
		{
			BAG dummy{};
			if (m_ptr != other.m_ptr)
			{
				free(m_ptr);

				max_size = other.max_size;
				m_ptr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
				next_pos = other.next_pos;
				valid = other.valid;
				if (!m_ptr)return dummy;
				else
				{
					if (next_pos > 0)for (size_t count = 0; count < next_pos; ++count)m_ptr[count] = other.m_ptr[count];
					other.m_ptr = nullptr;
					return (*this);
				}
			}
			return dummy;
		}

		T operator[](size_t index)
		{
			T dummy{};
			if (index < 0 || index >= next_pos)return dummy;

			return m_ptr[index];
		}

		bool empty()const
		{
			if (next_pos == 0)return true;

			return false;
		}
		size_t size()const
		{
			return next_pos;
		}
		size_t capacity()const
		{
			return max_size;
		}
		bool has_elements()const
		{
			return valid;
		}
	};

	class GRIDENG_API RANDIT
	{
	private:
		std::mt19937* twister{ nullptr };

	public:
		RANDIT();
		~RANDIT();

		int operator ()(int min, int max);
		float operator ()(float min, float max);
	};

	class GRIDENG_API PROTON
	{
	protected:
		float _width{ 0 };
		float _height{ 0 };

	public:
		FPOINT start{};
		FPOINT end{};
		FPOINT center{};
		float x_radius{ 0 };
		float y_radius{ 0 };

		PROTON();
		PROTON(float firstx, float firsty);
		PROTON(float firstx, float firsty, float firstw, float firsth);

		virtual ~PROTON() {};

		void set_edges();
		void new_dims(float new_width, float new_height);
		void set_width(float new_width);
		void set_height(float new_height);

		float get_width()const;
		float get_height()const;
	};

	class GRIDENG_API GRID
	{
	private:
		TILE grid[GRID_MAX_ROWS][GRID_MAX_COLS]{};

		RANDIT RandIt{};

		void add_row(dirs where);

		void add_col(dirs where);

	public:
		GRID();

		tiles get_type(int row, int col)const;

		void set_type(tiles what_type, int row, int col);

		float get_move_reduct(int row, int col)const;

		FRECT get_dims(int row, int col)const;

		void move(dirs to_where, float gear);
	};

	class GRIDENG_API PIGS :public PROTON
	{
	protected:
		pigs _type{ pigs::hero };

		float speed{ 0 };

		int max_frames{ 0 };
		int frame_delay{ 0 };
		int max_frame_delay{ 0 };
		int frame{ 0 };

		float move_sx{ 0 };
		float move_ex{ 0 };
		float move_sy{ 0 };
		float move_ey{ 0 };

		float slope{ 0 };
		float intercept{ 0 };

		bool hor_dir{ false };
		bool ver_dir{ false };

		PIGS(pigs _what_type, float _start_x, float _start_y);

	public:
		dirs dir = dirs::stop;
		int lifes = 100;

		int get_frame();
		pigs get_type()const;
		void move(float gear);
		void Release();

		void AIMove(BAG<FPOINT>& FoodBag, BAG<FPOINT>& ObstBag, FPOINT HeroPig, float game_speed);

		unsigned char Collision(FRECT myRect, FRECT ObstRect);
		void SetPath(float _endx, float _endy);

		static PIGS* create(pigs what_type, float start_x, float start_y);
	};

	class GRIDENG_API FOOD :public PROTON
	{
	private:
		food _type{ food::rotten };
		float _speed{ 1.0f };

		FOOD(food _what_type, float _first_x, float _first_y);

	public:

		food get_type()const;
		void move(dirs dir, float gear);
		void Release();

		static FOOD* create(food what_type, float first_x, float first_y);
	};

	class GRIDENG_API OBSTACLES :public PROTON
	{
	private:
		obstacles _type{ obstacles::rock };
		float _speed{ 1.0f };

		OBSTACLES(obstacles _what_type, float _first_x, float _first_y);

	public:

		obstacles get_type()const;
		void move(dirs dir, float gear);
		void Release();

		static OBSTACLES* create(obstacles what_type, float first_x, float first_y);
	};

	// FUNCTIONS **************************

	GRIDENG_API bool Intersect(FRECT first, FRECT second);
	GRIDENG_API bool Intersect(FPOINT first, FPOINT second, float first_x_rad, float second_x_rad,
		float first_y_rad, float second_y_rad);

	GRIDENG_API float Distance(FPOINT first, FPOINT second);

	GRIDENG_API void Sort(BAG<FPOINT>& container, FPOINT target);
}