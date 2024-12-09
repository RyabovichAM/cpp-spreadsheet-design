#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::unordered_set<Cell*>& GetCellsOnDepends();

private:
//можете воспользоваться нашей подсказкой, но это необязательно.
    class Impl {
    public:
        virtual ~Impl() = default;

        virtual CellInterface::Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        void CacheInvalidate() = 0;
    };

    class EmptyImpl : public Impl {
    public:
        EmptyImpl() = default;

        virtual CellInterface::Value GetValue() const {
            return "";
        }

        virtual std::string GetText() const {
            return "";
        }

        void CacheInvalidate() override;

    };
    class TextImpl : public Impl {
    public:
        TextImpl(std::string text) : text_{std::move(text)} {

        }

        virtual CellInterface::Value GetValue() const {
            if(text_[0] == '\'') {
                return text_.substr(1);
            }
            return text_;
        }
        virtual std::string GetText() const {
            return text_;
        }

        void CacheInvalidate() override;

        private:
            std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::string formula) : 
            formula_{ParseFormula(std::move(formula))} {

        }

        virtual ~FormulaImpl() = default;

        virtual CellInterface::Value GetValue() const {
            auto res = formula_->Evaluate();
            if(std::holds_alternative<double>(res)) {
                return std::get<double>(res);
            }
            return std::get<FormulaError>(res);
        }

        virtual std::string GetText() const {
            return {'=' + formula_->GetExpression()};
        }

        void CacheInvalidate() override;

    private:
        std::unique_ptr<FormulaInterface> formula_;
        std::optional<CellInterface::Value> cache_;
    };


    std::unique_ptr<Impl> impl_;
    std::unordered_set<Cell*>  dependency_cells_;
    std::unordered_set<Cell*>  cells_on_depends_;

};
