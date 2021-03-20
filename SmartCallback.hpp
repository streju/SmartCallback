#include <functional>
#include <cassert>

template <typename>
class SmartCallback;

template <typename ResultType, typename ...Args>
class SmartCallback<ResultType(Args...)>
{
public:
    // Explicit delete creation of not callable object. Compile time check.
    SmartCallback() = delete;
    explicit SmartCallback(std::nullptr_t) noexcept = delete;
    SmartCallback& operator=(std::nullptr_t) = delete;
    explicit operator bool() const noexcept = delete;

    // Constructor with callable functor. Callable check in std::function ctor 
    template <typename FunctorT>
	SmartCallback(FunctorT callable)
        : ctrlPtr_(new NotSoSmartStruct(std::forward<FunctorT>(callable)))
    {}

    SmartCallback(const SmartCallback& rhs)
    {
        this->ctrlPtr_ = rhs.ctrlPtr_;
        this->ctrlPtr_->referenceCounter_++;
    }

    SmartCallback& operator=(const SmartCallback& rhs)
    {
        if (this->ctrlPtr_ && --(this->ctrlPtr_->referenceCounter_) == 0)
        {
            bool wasCalled = ctrlPtr_->wasCalled_;
            delete ctrlPtr_;
            assert(wasCalled && "Callback wasn't called!"); 
        }

        this->ctrlPtr_ = rhs.ctrlPtr_;
        this->ctrlPtr_->referenceCounter_++;
        return *this;
    }

    SmartCallback(SmartCallback&& rhs)
    {
        this->ctrlPtr_ = rhs.ctrlPtr_;
        rhs.ctrlPtr_ = nullptr;
    }

    SmartCallback& operator=(SmartCallback&& rhs)
    {
        if (this->ctrlPtr_ && --(this->ctrlPtr_->referenceCounter_) == 0)
        {
            bool wasCalled = ctrlPtr_->wasCalled_;
            delete ctrlPtr_;
            assert(wasCalled && "Callback wasn't called!"); 
        }

        this->ctrlPtr_ = rhs.ctrlPtr_;
        rhs.ctrlPtr_ = nullptr;
        return *this;
    }

    ~SmartCallback()
    {
        if (ctrlPtr_ && --ctrlPtr_->referenceCounter_ == 0)
        {
            bool wasCalled = ctrlPtr_->wasCalled_;
            delete ctrlPtr_;
            assert(wasCalled && "Callback wasn't called!"); 
        }     
    }

    ResultType operator()(Args... args)
    {
        assert(isValid() && "There is no callback!");

        ctrlPtr_->wasCalled_ = true;
        return ctrlPtr_->callback_(args...);
    }
    
    unsigned useCount()
    {
        if (!ctrlPtr_) return 0;
        return ctrlPtr_->referenceCounter_;
    }

    bool isValid()
    {
        return ctrlPtr_ != nullptr;
    }

    bool wasCalled()
    {
        return ctrlPtr_ && ctrlPtr_->wasCalled_;
    }

private:
	struct NotSoSmartStruct
	{
        template <typename FunctorT>
        NotSoSmartStruct(FunctorT&& callableObj) : callback_(std::forward<FunctorT>(callableObj))
        {}
        std::function<ResultType(Args...)> callback_;
		bool wasCalled_{false}; // flag needed, there is no similar check in std::function (function can be called multiple times)
		unsigned referenceCounter_{1};
	};

    NotSoSmartStruct* ctrlPtr_;
};