// heap.h (Binary heap)

#include<vector>
#include<stdexcept>

template<class Object>         // assume that Object has the < operator for comparison
class Heap
{
public:
      size_t num_elements() const
      {
            return _data.size();
      }

      bool empty() const
      {
            return (_data.size() == 0);
      }

      Object find_min() const
      {
            if (not empty()) return _data[0];
            else throw std::runtime_error("Empty heap; Heap::find_min failed.");
      }
    
      Object extract_min()
      {
            Object result = find_min();
            remove(0);
            return result;
      }
    
      void insert(Object object)
      {
            _data.push_back(object);
            sift_up(_data.size()-1);
      }

protected:                                      // accessible only for derived classes
      void remove(size_t index)
      {
            if (index < _data.size())
            {
                  swap(_data[index], _data[_data.size()-1]);
                  _data.pop_back();
                  sift_up(index);
                  sift_down(index);
            }
            else throw std::runtime_error("Index error; Heap::remove failed");
      }

      void decrease_key(size_t index)
      {
            if (index < _data.size()) sift_up(index);
            else throw std::runtime_error("Index error; Heap::decrease_key failed.");
      }

      virtual void swap (Object & a, Object & b)      // virtual functions can be
      {                                               // overloaded by derived classes
            std::swap(a,b); 
      }

      static size_t parent(size_t index)              // do not call with index==0!
      { 
            return ((index - 1) / 2); 
      } 

      static size_t left(size_t index)                // left child may not exist!
      { 
            return (2 * index) + 1; 
      } 

      static size_t right(size_t index)               // right child may not exist!
      { 
            return (2 * index) + 2; 
      }  

      void sift_up(size_t index)
      {
            while ((index > 0) and (_data[index] < _data[parent(index)]))
            {
                  swap(_data[index], _data[parent(index)]);
                  index = parent(index);
            }
      }
    
      void sift_down(size_t index)
      {
            size_t smallest = index;
            while (true)
            {
                  if ((left(index) < _data.size()) and 
                      (_data[left(index)] < _data[smallest]))
                  {
                        smallest = left(index);
                  }
                  if ((right(index) < _data.size()) and 
                      (_data[right(index)] < _data[smallest]))
                  {
                        smallest = right(index);
                  }
                  if (index == smallest) return;
                  swap(_data[smallest], _data[index]);
                  index = smallest;
            } 
      }

      std::vector<Object> _data;                    // holds the objects in heap order
};    

